<?php

/**
 * GoPay HTTP Notification callback.
 */
function commerce_gopay_soap_notify() {
  $returned_ipn = array(
    'payment_session_id' => $_GET['paymentSessionId'],
    'go_id' => $_GET['eshopGoId'],
    'variable_symbol' => $_GET['variableSymbol'],
    'encrypted_signature' => $_GET['encryptedSignature'],
  );

  //watchdog('commerce_gopay', "Fake IPN processed with session id @payment_session_id.", array('@payment_session_id' => $returned_ipn['payment_session_id']), WATCHDOG_DEBUG);
  //commerce_gopay_soap_notify_success();

  $ipn = commerce_gopay_ipn_load($returned_ipn['payment_session_id']);

  if ($ipn === FALSE) {
    watchdog('commerce_gopay', "Couldn't load IPN for session id @payment_session_id.", array('@payment_session_id' => $returned_ipn['payment_session_id']), WATCHDOG_WARNING);
    commerce_gopay_soap_notify_failed();
  }

  if (!empty($ipn['payment_method'])) {
    $payment_method = commerce_payment_method_instance_load($ipn['payment_method']);
  }
  else {
    watchdog('commerce_gopay', "Couldn't load payment method for IPN with session id @payment_session_id.", array('@payment_session_id' => $returned_ipn['payment_session_id']), WATCHDOG_WARNING);
    commerce_gopay_soap_notify_failed();
  }

  if ($ipn['order_id']) {
    $order = commerce_order_load($ipn['order_id']);
  }
  else {
    watchdog('commerce_gopay', "Couldn't load order associated with IPN with session id @payment_session_id.", array('@payment_session_id' => $returned_ipn['payment_session_id']), WATCHDOG_WARNING);
    commerce_gopay_soap_notify_failed();
  }

  // Give the payment method module an opportunity to validate IPN.
  // If function exists, it is responsible for setting its own watchdog message.
  $callback = $payment_method['base'] . '_ipn_validate';

  // If a validator function existed...
  if (function_exists($callback)) {
    // Only exit if the function explicitly returns FALSE.
    if ($callback($order, $payment_method, $ipn, $returned_ipn) === FALSE) {
      commerce_gopay_soap_notify_failed();
    }
  }

  $callback = $payment_method['base'] . '_ipn_process';

  // If a processing function existed...
  if (function_exists($callback)) {
    // Skip saving if the function explicitly returns FALSE, meaning the IPN
    // wasn't actually processed.
    if ($callback($order, $payment_method, $ipn) !== FALSE) {
      // Save the processed IPN details.
      commerce_gopay_ipn_save($ipn);
      commerce_gopay_soap_notify_success();
    }
    else {
      commerce_gopay_soap_notify_failed();
    }
  }

  watchdog('commerce_gopay', "IPN wasn't processed. There's no implementation of @function", array('@function' => $callback), WATCHDOG_WARNING);
  commerce_gopay_soap_notify_failed();
}

/**
 * Get a payment status and message based on the GoPay status.
 * 
 * @param $ipn
 *   Received GoPay status ID.
 */
function commerce_gopay_feedback_status($ipn, &$transaction) {
  switch ($ipn['payment_status']) {
    case 'PAYMENT_DONE':
      $transaction->status = COMMERCE_PAYMENT_STATUS_SUCCESS;
      $transaction->message = t("The payment has completed.");
      break;

    case 'CANCELED':
      $transaction->status = COMMERCE_PAYMENT_STATUS_FAILURE;
      $transaction->message = t('The payment was canceled.');
      break;

    case 'TIMEOUTED':
      $transaction->status = COMMERCE_PAYMENT_STATUS_FAILURE;
      $transaction->message = t('The payment was canceled.');
      break;

    case 'WAITING':
      $transaction->status = COMMERCE_PAYMENT_STATUS_PENDING;
      $transaction->message = t('Pending.');
      break;
  }
}

function commerce_gopay_soap_notify_failed() {
  header('HTTP/1.1 500 Internal Server Error');
  exit(0);
}

function commerce_gopay_soap_notify_success() {
  header('HTTP/1.1 200');
  exit(0);
}
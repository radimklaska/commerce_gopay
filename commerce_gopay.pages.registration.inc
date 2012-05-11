<?php

//=============================================================================
//= MENU CALLBACKS
//=============================================================================


/**
 * Optional registration form.
 */
function commerce_gopay_registration_form($form, &$form_state) {

  $form['commerce_gopay_back_top'] = array(
    '#prefix' => '<p>',
    '#markup' => l(t('Back to GoPay settings'), 'admin/commerce/config/payment-methods/manage/commerce_payment_gopay'),
    '#suffix' => '</p>',
  );

  $registered = variable_get('commerce_gopay_registration', FALSE);
  $defaults = variable_get('commerce_gopay_registration_data', array(
                               'first_name' => '',
                               'second_name' => '',
                               'eshop_url' => '',
                               'email' => '',
                               'goid' => '',
                               'last_update' => '',
                           ));

  if ($registered == FALSE) {
    $form['appeal_title'] = array(
      '#type' => 'markup',
      '#markup' => t("Please, support the development and register this module!"),
      '#prefix' => '<h3>',
      '#suffix' => '</h3>',
    );
    $form['appeal_body'] = array(
      '#type' => 'markup',
      '#markup' => t(<<<TXT
This module isn't officially supported by the GoPay s.r.o., but GoPay will pay
us for every site which is using this module. Unfortunately GoPay provider has
no tool to detect what plugin each e-shop is using and the whole evidence of
these sites is upon to us. The registration is optional and its done by an
email. You can use the form below or you can send an email to
<a href="mailto:@email">@email</a> with the following data: e-shop owner name,
owner e-mail address, e-shop URL and GoPay ID (if you've already obtained it).
TXT
, array('@email' => commerce_gopay_REGISTRATION_EMAIL)),
      '#prefix' => '<p>',
      '#suffix' => '</p>',
    );
  }
  else {
    $form['thankyou'] = array(
      '#type' => 'markup',
      '#markup' => t("Thank you for your support!"),
      '#prefix' => '<h3>',
      '#suffix' => '</h3>',
    );
    $form['last_change'] = array(
      '#type' => 'markup',
      '#markup' => t("The registration was sent @date", array('@date' => format_date($registered))),
      '#prefix' => '<p>',
      '#suffix' => '</p>',
    );
  }
  $form['disclaimer_title'] = array(
    '#type' => 'markup',
    '#markup' => t("Disclaimer"),
    '#prefix' => '<h3>',
    '#suffix' => '</h3>',
  );
  $form['disclaimer_body'] = array(
    '#type' => 'markup',
    '#markup' => t("We guarantee you that the following information will be used
 only for evidence of the sites which are using this module and will be provided to GoPay s.r.o.
 This information will not be shared to any other subject."),
    '#prefix' => '<p>',
    '#suffix' => '</p>',
  );
  $form['first_name'] = array(
    '#type' => 'textfield',
    '#title' => t('First name'),
    '#required' => TRUE,
    '#default_value' => $defaults['first_name'],
    '#description' => t('First name of the e-shop owner.') . ' ' .
                      t('Should be same as the one used in GoPay official registration process.'),
  );
  $form['second_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Second name'),
    '#required' => TRUE,
    '#default_value' => $defaults['second_name'],
    '#description' => t('Second name of the e-shop owner.') . ' ' .
                      t('Should be same as the one used in GoPay official registration process.'),
  );
  $form['email'] = array(
    '#type' => 'textfield',
    '#title' => t('E-mail'),
    '#required' => TRUE,
    '#default_value' => $defaults['email'] ? $defaults['email'] : variable_get('site_mail', ''),
    '#description' => t('E-mail of the e-shop owner.') . ' ' .
                      t('Should be same as the one used in GoPay official registration process.'),
  );
  $form['eshop_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Eshop URL'),
    '#required' => TRUE,
    '#default_value' => $defaults['eshop_url'] ? $defaults['eshop_url'] : url('<front>', array('absolute' => TRUE)),
    '#description' => t('URL of your e-shop including http:// or https://.'),
  );
  $form['goid'] = array(
    '#type' => 'textfield',
    '#title' => t('GoPay ID'),
    '#required' => FALSE,
    '#default_value' => '',
    '#description' => t('Your GoPay ID, if you have pass the official registration.'),
  );
  $form['send_copy'] = array(
    '#type' => 'checkbox',
    '#title' => t('Send copy'),
    '#description' => t('Send copy to the provided e-mail address. (Includes the registrar email in the blind carbon copy.)'),
    '#default_value' => '',
  );
  $form['warning'] = array(
    '#type' => 'markup',
    '#markup' => t("Please note, that this is not official e-shop registration, you still need to contact the official GoPay provider to use the GoPay payment gateway."),
    '#prefix' => '<p><strong>',
    '#suffix' => '</strong></p>',
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['preview'] = array(
    '#type' => 'submit',
    '#value' => t('Preview'),
  );
  $form['actions']['send'] = array(
    '#type' => 'submit',
    '#value' => t('Send'),
  );

  $form['actions']['commerce_gopay_back_bottom'] = array(
    '#prefix' => '<p>',
    '#markup' => l(t('Back to GoPay settings'), 'admin/commerce/config/payment-methods/manage/commerce_payment_gopay'),
    '#suffix' => '</p>',
  );

  return $form;
}


/**
 * An form validation handler.
 *
 * @see commerce_gopay_registration_form()
 */
function commerce_gopay_registration_form_validate(&$form, &$form_state) {
  $values =& $form_state['values'];

  // Validate the e-mail address.
  if ($error = user_validate_mail($values['email'])) {
    form_set_error('email', $error);
  }
  // Validate eshop URL. See http://mathiasbynens.be/demo/url-regex.
  if (!empty($values['eshop_url']) && !preg_match('_^(?:(?:https?|ftp)://)(?:\S+(?::\S*)?@)?(?:(?!10(?:\.\d{1,3}){3})(?!127(?:\.\d{1,3}){3})(?!169\.254(?:\.\d{1,3}){2})(?!192\.168(?:\.\d{1,3}){2})(?!172\.(?:1[6-9]|2\d|3[0-1])(?:\.\d{1,3}){2})(?:[1-9]\d?|1\d\d|2[01]\d|22[0-3])(?:\.(?:1?\d{1,2}|2[0-4]\d|25[0-5])){2}(?:\.(?:[1-9]\d?|1\d\d|2[0-4]\d|25[0-4]))|(?:(?:[a-z\x{00a1}-\x{ffff}0-9]+-?)*[a-z\x{00a1}-\x{ffff}0-9]+)(?:\.(?:[a-z\x{00a1}-\x{ffff}0-9]+-?)*[a-z\x{00a1}-\x{ffff}0-9]+)*(?:\.(?:[a-z\x{00a1}-\x{ffff}]{2,})))(?::\d{2,5})?(?:/[^\s]*)?$_iuS', $values['eshop_url'])) {
    form_set_error('eshop_url', t('The e-shop URL %url is not valid.', array('%url' => $values['eshop_url'])));
  }
}

/**
 * An form submit handler.
 *
 * @see commerce_gopay_registration_form()
 */
function commerce_gopay_registration_form_submit(&$form, &$form_state) {
  $values =& $form_state['values'];

  $to = commerce_gopay_REGISTRATION_EMAIL;
  $from = variable_get('site_mail', '');
  $language = language_default();
  $params = $values;
  $params['registered'] = REQUEST_TIME;
  $send = $form['actions']['send']['#value'] === $values['op'];
  $preview = $form['actions']['preview']['#value'] === $values['op'];

  $message = drupal_mail('commerce_gopay','register', $to, $language, $params, $from, $send);

  if ($send && $message['result']) {
    drupal_set_message('Thank you for the registration! We really appreciate it!');
    variable_set('commerce_gopay_registration', REQUEST_TIME);
    // Store registration data
    $data = array(
      'first_name' => $values['first_name'],
      'second_name' => $values['second_name'],
      'eshop_url' => $values['eshop_url'],
      'email' => $values['email'],
      'goid' => $values['goid'],
      'last_update' => REQUEST_TIME,
    );
    variable_set('commerce_gopay_registration_data', $data);
  }
  elseif ($preview) {
    dpm($message);
    drupal_set_message(t('<b>Message preview</b><br><pre><b>From:</b> @from<br><b>To:</b> @to<br><b>Subject:</b> @subject<br><b>Body:</b><br>@body</pre>', array('@from' => $message['from'], '@to' => $message['to'], '@subject' => $message['subject'], '@body' => $message['body'])));
    $form_state['rebuild'] = TRUE;
  }
  else {
    $form_state['rebuild'] = TRUE;
  }
}

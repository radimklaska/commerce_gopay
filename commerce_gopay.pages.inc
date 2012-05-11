<?php

//=============================================================================
//= MENU CALLBACKS
//=============================================================================

/**
 * Successful payment callback.
 */
function commerce_gopay_payment_success() {
  //TODO
  watchdog('commerce_gopay', 'commerce_gopay_payment_success');
}

/**
 * Failed payment callback.
 */
function commerce_gopay_payment_failed() {
  //TODO
  watchdog('commerce_gopay', 'commerce_gopay_payment_failed');
}

/**
 * Notify callback.
 */
function commerce_gopay_soap_notify() {
  //TODO
  watchdog('commerce_gopay', 'commerce_gopay_soap_notify');
}
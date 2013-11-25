<?php
/**
 * Generate url response and confirmation.
 */
function commerce_payulatam_get_url($order_id) {
  $token = uniqid();

  $url_response = COMMERCE_PAYULATAM_URL_RESPONSE . '/' . $order_id . '/' . $token . '/' . commerce_payulatam_get_md5($order_id, $token, 'RESPONSE');

  $url_confirmation = COMMERCE_PAYULATAM_URL_CONFIRMATION . '/' . $order_id . '/' . $token . '/' . commerce_payulatam_get_md5($order_id, $token);

  return array(
    'response' => url($url_response, array(
      'absolute' => TRUE,
    )),
    'confirmation' => url($url_confirmation, array(
      'absolute' => TRUE,
    )),
  );
}

/**
 * Generate number reference unique.
 */
function commerce_payulatam_get_reference($alias, $order_id) {
  return $alias . $order_id;
}

/**
 * Digital signature of the transaction.
 */
function commerce_payulatam_get_firm($settings, $state_transation = "") {
  $params = array(
    $settings['po_encryption_key'],
    $settings['po_uid'],
    $settings['sale_reference'],
    $settings['amount'],
    $settings['currency_code'],
  );
  if (!empty($state_transation)) {
    $params[] = $state_transation;
  }
  return md5(implode('~', $params));
}

/**
 * Encrypted key.
 */
function commerce_payulatam_get_md5($order_id, $token, $type = 'CONFIRMATION') {
  return md5($order_id . '~' . strtoupper($type) . '~' . $token);
}

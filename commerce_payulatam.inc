<?php
/**
 * @file
 * API payment payulatam
 */

/**
 * Page response.
 */
function commerce_payulatam_response($order) {
  $param = commerce_payulatam_get_param($_GET);

  if($param['transactionState'] == 6 && $param['polResponseCode'] == 5) {
    $message = 'Failed transaction';
  }
  else if($param['transactionState'] == 6 && $param['polResponseCode'] == 4) {
    $message = 'Transaction rejected';
  }
  else if($param['transactionState'] == 12 && $param['polResponseCode'] == 9994) {
    $message = 'Pending, Please check whether the debit was made in the Bank';
  }
  else if($param['transactionState'] == 4 && $param['polResponseCode'] == 1) {
    $message = 'Transaction approved';
  }
  else {
    $message = $param['mensaje'];
  }

  $rows = array(
    array(t('Transaction state'), $message),
    array(t('Transaction ID'), $param['transactionId']),
    array(t('Sale reference'), $param['reference_pol']),
    array(t('Transaction Reference'), $param['referenceCode']),
    array(t('Ammount'), number_format($param['TX_VALUE'])),
    array(t('Currency'), $param['currency']),
    array(t('Description'), $param['description']),
    array(t('Entity'), $param['lapPaymentMethod']),
  );

  if ($param['pseBank']) {
    $rows[] = array(t('CUS'), $param['cus']);
    $rows[] = array(t('Bank'), $param['pseBank']);
  }

  $content['table'] = array(
    '#theme' => 'table',
    '#rows' => $rows,
  );

  drupal_alter('commerce_payulatam_response', $content, $param);

  return $content;
}

/**
 * Page confirmation.
 */
function commerce_payulatam_confirmation($order) {
  $param = commerce_pagosonline_get_param($_POST);
  $transaction = array();
  
  if ($param['estado_pol'] == 5) {
    commerce_payment_redirect_pane_previous_page($order);
  }
  else {
    $transaction = commerce_pagosonline_save_transation($param, $order);
    if ($transaction->status == COMMERCE_PAYMENT_STATUS_SUCCESS) {
      commerce_payment_redirect_pane_next_page($order);
    }
  }

  module_invoke_all('commerce_payulatam_confirmation', $order, $transaction);
  $result = array(
    'estate' => TRUE,
  );
  drupal_json_output($result);
}

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
function commerce_payulatam_get_firm($settings, $state_transation = NULL) {
  $params = array(
    $settings['py_api_key'],
    $settings['py_merchant_id'],
    $settings['sale_reference'],
    $settings['amount'],
    $settings['currency_code'],
  );

  if ($state_transation) {
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

/**
 * Load parameters.
 */
function commerce_payulatam_get_param($target) {
  $param = array();
  if (is_array($target)) {
    foreach ($target as $key => $value) {
      $param[$key] = check_plain($target[$key]);
    }
  }
  return $param;
}
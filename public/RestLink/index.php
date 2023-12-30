<?php

if (isset($_GET['app_id']))
    $appId = $_GET['app_id'];
else
    $appId = '';

if (isset($_GET['app_version']))
    $appVersion = $_GET['app_version'];
else
    $appVersion = '';

if ($appId == '64d68394-7efc-11ee-abd5-1353e606e9ea') {
    // Marvel Discovery
    $appName = 'Marvel Discovery';
    $configFile = 'Marvel/Discovery/TMDB3.json';
} else if ($appId == '47cb8b8c-7efc-11ee-905d-836d044cadfc') {
    // Blagues
    $appName = 'Blagues';
    $configFile = 'Blagues/Blagues.json';
} else {
    // Unknown
    $appName = '';
    $configFile = null;
}

if ($configFile != null && file_exists($configFile)) {
    $json = json_decode(file_get_contents($configFile));
    $json->app = array(
        'id' => $appId,
        'name' => $appName
    );
    $code = 200;
} else {
    $json = array(
        'error' => 'An error occured during data fetch'
    );
    $code = 404;
}

echo json_encode($json);
http_response_code($code);

?>

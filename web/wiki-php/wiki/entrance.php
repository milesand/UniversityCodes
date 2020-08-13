<?PHP
$doc_root = rtrim($_SERVER['DOCUMENT_ROOT'], '/');
$json = file_get_contents("$doc_root/setting.json");
if ($json === FALSE) {
    error_log('Missing setting.json');
    http_response_code(500);
    exit();
}

$config = json_decode($json);
if (!isset($config->main_page)) {
    error_log('Missing main_page in setting.json');
    http_response_code(500);
    exit();
}
$main_page = $config->main_page;

http_response_code(302);
header("Location: /view/$main_page");
exit();
?>
<?php
session_start();

if (!isset($_SESSION['username'])) {
    http_response_code(400);
    exit();
}

unset($_SESSION['username']);
unset($_SESSION['user_id']);

if (isset($_GET['redirect'])) {
    http_response_code(302);
    $redirect = urldecode($_GET['redirect']);
    header("Location: $redirect");
    exit();
}
$doc_root = $_SERVER['DOCUMENT_ROOT'];
include("$doc_root/entrance.php");
exit();
?>
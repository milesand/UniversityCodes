<?php
session_start();

if (!isset($_POST['username'])) {
    http_response_code(400);
    exit();
}

if (!isset($_SESSION['username'])) {
    $doc_root = rtrim($_SERVER['DOCUMENT_ROOT'], '/');
    include_once("$doc_root/snippets/db.php");
    $mysqli = new DBConnection();
    $stmt = $mysqli->prepare('SELECT `user`.`id` FROM `user` WHERE `user`.`name` = ? AND `user`.`registered` = 1 LIMIT 1');
    $stmt->bind_param('s', $_POST['username']);
    $stmt->execute();
    $stmt->bind_result($user_id);
    $result = $stmt->fetch();
    $stmt->close();
    $mysqli->close();
    if ($result) {
        $_SESSION['username'] = $_POST['username'];
        $_SESSION['user_id'] = $user_id;
    } else {
        $_SESSION['login_fail'] = TRUE;
        http_response_code(302);
        if($_SERVER['QUERY_STRING'] !== '') {
            $query = $_SERVER['QUERY_STRING'];
            header("Location: /login?$query");
        } else {
            header("Location: /login");
        }
        exit();
    }
}

if (isset($_GET['redirect'])) {
    http_response_code(302);
    $redirect = urldecode($_GET['redirect']);
    header("Location: $redirect");
    exit();
}
$doc_root = rtrim($_SERVER['DOCUMENT_ROOT'], '/');
include("$doc_root/entrance.php");
exit();
?>
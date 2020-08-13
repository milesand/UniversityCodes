<?php
$doc_root = rtrim($_SERVER['DOCUMENT_ROOT'], '/');
include_once("$doc_root/snippets/encoding.php");

session_start();

if (!isset($_POST['username'])) {
    http_response_code(400);
    exit();
}
if (ensure_utf8($_POST['username']) !== $_POST['username']) {
    http_response(400);
    exit();
}
if (strlen($_POST['username']) > 255) {
    $username = mb_strimwidth($_POST['username'], 0, 255, "", 'UTF-8');
} else {
    $username = $_POST['username'];
}

if (!isset($_SESSION['username'])) {
    $doc_root = rtrim($_SERVER['DOCUMENT_ROOT'], '/');
    include_once("$doc_root/snippets/db.php");
    $mysqli = new DBConnection();
    $mysqli->begin_transaction(MYSQLI_TRANS_START_READ_WRITE);
    $stmt = $mysqli->prepare('SELECT `user`.`id` FROM `user` WHERE `user`.`name` = ? AND `user`.`registered` = 1 LIMIT 1');
    $stmt->bind_param('s', $username);
    $stmt->execute();
    $stmt->bind_result($user_id);
    $result = $stmt->fetch();
    $stmt->close();
    if ($result) {
        $mysqli->commit();
        $mysqli->close();
        $_SESSION['join_fail'] = TRUE;
        http_response_code(302);
        if($_SERVER['QUERY_STRING'] !== '') {
            $query = $_SERVER['QUERY_STRING'];
            header("Location: /join?$query");
        } else {
            header("Location: /join");
        }
        exit();
    } else {
        $stmt = $mysqli->prepare('INSERT INTO `user` (`name`, `registered`) VALUES (?, 1)');
        $stmt->bind_param('s', $username);
        $stmt->execute();
        $stmt->close();
        $_SESSION['username'] = $username;
        $_SESSION['user_id'] = $mysqli->insert_id;
        $mysqli->commit();
        $mysqli->close();
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
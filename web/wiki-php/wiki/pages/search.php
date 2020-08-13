<?php
if (!isset($_GET['title'])) {
    http_response_code(400);
    die();
}

$doc_root = rtrim($_SERVER['DOCUMENT_ROOT'], '/');
include_once("$doc_root/snippets/db.php");
include_once("$doc_root/snippets/format.php");
include_once("$doc_root/snippets/title.php");

$title = new Title($_GET['title']);
$title_raw = $title->raw();

$mysqli = new DBConnection();
$stmt = $mysqli->prepare('SELECT `page`.`id` FROM `page` WHERE `namespace` = ? AND `name` = ? LIMIT 1');
$stmt->bind_param('is', $title->namespace, $title->name);
$stmt->execute();
$stmt->bind_result($_);
$result = $stmt->fetch();
$mysqli->close();
if ($result === TRUE) {
    $contents = "<p>There is a page named <a href=\"/view/$title_raw\"><strong>$title_raw</strong></a> in this wiki.</p>";
} else {
    $contents = "<p>There is no page named <a href=\"/edit/$title_raw\" class=\"invalid-internal-link\"><strong>$title_raw</strong></a> in this wiki, but you can create one.</p>";
}

print format_page('Search', LOGIN_REDIRECT, NULL, $contents);
?>
<?php
if (!isset($_GET['title'])) {
    http_response_code(400);
    die();
}

$doc_root = $_SERVER['DOCUMENT_ROOT'];
include_once("$doc_root/snippets/db.php");
include_once("$doc_root/snippets/title.php");

$title = new Title($_GET['title']);

$mysqli = new DBConnection();
$mysqli->begin_transaction(MYSQLI_TRANS_START_READ_ONLY);
$stmt = $mysqli->prepare(<<<SQL
    SELECT `page`.`id`
    FROM `page`
    WHERE `page`.`namespace` = ? AND `page`.`name` = ?
SQL
);
$stmt->bind_param('is', $title->namespace, $title->name);
$stmt->execute();
$stmt->bind_result($page_id);
$result = $stmt->fetch();
$stmt->close();
if ($result !== TRUE) {
    $mysqli->close();
    http_response_code(400);
    exit();
}
if (isset($_GET['rev'])) {
    $revision = intval($_GET['rev']);
    $stmt = $mysqli->prepare(<<<SQL
        SELECT `text`.`md`
        FROM
            (
                SELECT `revision`.`text_id`
                FROM `revision`
                WHERE
                    `revision`.`page_id` = ?
                    AND `revision`.`revision_no` = ?
                LIMIT 1
            ) AS `revision`
            JOIN `text` ON `text`.`id` = `revision`.`text_id`
SQL
    );
    $stmt->bind_param('ii', $page_id, $revision);
    $stmt->execute();
    $stmt->bind_result($md);
    $result = $stmt->fetch();
    $stmt->close();
    if ($result !== TRUE) {
        $mysqli->close();
        http_response_code(400);
        exit();
    }
} else {
    $stmt = $mysqli->prepare(<<<SQL
        SELECT `text`.`md`
        FROM
            (
                SELECT `page`.`last_rev`
                FROM `page`
                WHERE `page`.`id` = ?
            ) AS `page`
            JOIN `revision` ON `revision`.`id` = `page`.`last_rev`
            JOIN `text` ON `text`.`id` = `revision`.`text_id`
SQL
    );
    $stmt->bind_param('i', $page_id);
    $stmt->execute();
    $stmt->bind_result($md);
    $result = $stmt->fetch();
    $stmt->close();
    if ($result !== TRUE) {
        $mysqli->close();
        http_response_code(500);
        die();
    }
}

$mysqli->commit();

header('Content-Type: text/plain');
print $md;
?>
<?php
if (!isset($_GET['title']) || !isset($_GET['revision'])) {
    http_response_code(400);
    die();
}

$doc_root = $_SERVER['DOCUMENT_ROOT'];
include_once("$doc_root/snippets/format.php");
include_once("$doc_root/snippets/db.php");
include_once("$doc_root/snippets/update.php");
include_once("$doc_root/snippets/title.php");

$title = new Title($_GET['title']);
$title_raw = $title->raw();
$revision = intval($_GET['revision']);

$mysqli = new DBConnection();
$stmt = $mysqli->prepare(<<<SQL
SELECT
    `last_revision`.`revision_no`,
    `text`.`html`
FROM
    (
        SELECT
            `page`.`id`,
            `page`.`last_rev`
        FROM
            `page`
        WHERE
            `page`.`namespace` = ?
            AND `page`.`name` = ?
        LIMIT 1
    ) AS `page`
    JOIN (
        SELECT
            `revision`.`id`,
            `revision`.`revision_no`
        FROM
            `revision`
        ) AS `last_revision`
        ON `last_revision`.`id` = `page`.`last_rev`
    JOIN (
        SELECT
            `revision`.`page_id`,
            `revision`.`revision_no`,
            `revision`.`text_id`
        FROM
            `revision`
        ) AS `old_revision`
        ON `old_revision`.`page_id` = `page`.`id`
        AND `old_revision`.`revision_no` = ?
    JOIN `text` ON `text`.`id` = `old_revision`.`text_id`
SQL
);
$stmt->bind_param('isi', $title->namespace, $title->name, $revision);
$stmt->execute();
$stmt->bind_result($last_revision, $html);
$res = $stmt->fetch();
$stmt->close();
$mysqli->close();
if ($res === FALSE) {
    http_response_code(500);
    die();
} else if ($res === NULL) {
    http_response_code(400);
    exit();
}

if ($revision === 1) {
    $buttons = '<span class="btn disabled"><strong>r0</strong></span>';
} else {
    $rev_before = $revision - 1;
    $buttons = "<a class=\"btn\" href=\"/revision/$title_raw/$rev_before\"><strong>r$rev_before</strong></a>";
}
$rev_after = $revision + 1;
if ($revision === $last_revision) {
    $buttons .= "<span class=\"btn disabled\"><strong>r$rev_after</strong></span>";
} else {
    $buttons .= "<a class=\"btn\" href=\"/revision/$title_raw/$rev_after\"><strong>r$rev_after</strong></a>";
}

$contents = format_article($title_raw, "(r$revision)", $buttons . $html, WRAP_IN_ARTICLE);

print format_page("$title_raw (r$revision)", LOGIN_REDIRECT,
    array('View' => "/view/$title_raw", 'History' => "/history/$title_raw"),
    $contents
);
?>

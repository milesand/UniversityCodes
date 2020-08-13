<?php
if (!isset($_GET['title'])) {
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

$mysqli = new DBConnection();
$stmt = $mysqli->prepare(<<<SQL
SELECT
    `page`.`id`,
    `page`.`last_render`,
    `revision`.`text_id`,
    `text`.`md`,
    `text`.`html`
FROM
    (
      SELECT
        `page`.`id`,
        `page`.`last_rev`,
        `page`.`last_render`
      FROM
        `page`
      WHERE
        `page`.`namespace` = ?
        AND `page`.`name` = ?
      LIMIT 1
    ) AS `page`
    JOIN `revision` ON `revision`.`id` = `page`.`last_rev`
    JOIN `text` ON `text`.`id` = `revision`.`text_id`
SQL
);

$stmt->bind_param('is', $title->namespace, $title->name);
$stmt->execute();
$stmt->bind_result($page_id, $last_render, $text_id, $md, $html);
$res = $stmt->fetch();
$stmt->close();
if ($res === NULL) {
    $contents = "<p>Page <a href=\"/edit/$title_raw\" class=\"invalid-internal-link\"><strong>$title_raw</strong></a> does not exist, but you can create one.</p>";
    print format_page($title_raw, LOGIN_REDIRECT, NULL, $contents);
    exit();
}
$last_render = strtotime($last_render);
$seconds_since_render = time() - $last_render;
if ($seconds_since_render > 10) {
    include_once("$doc_root/snippets/render.php");
    $html = render($md, $mysqli);
    update_text($mysqli, $page_id, $text_id, $html);
}

$contents = format_article($title_raw, NULL, $html, WRAP_IN_ARTICLE);
print format_page($title_raw, LOGIN_REDIRECT, array(
    'Edit' => "/edit/$title_raw",
    'History' => "/history/$title_raw"
), $contents);
?>
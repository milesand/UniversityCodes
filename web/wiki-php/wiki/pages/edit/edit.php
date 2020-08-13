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
    `text`.`md`
FROM
    (
        SELECT
            `page`.`last_rev`
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
$stmt->bind_result($md);
$res = $stmt->fetch();
$stmt->close();
if ($res === NULL) {
    $html = "<p>Page <strong>$title_raw</strong> does not exist. Saving will creating one.</p>";
    $md = "";
} else {
    $html = "";
}

$html .= <<<HTML
<form action="/submit/$title_raw" method="post">
<textarea class="editTextArea" name="markdown" cols="80" rows="25">$md</textarea>
<input type="submit" value="Submit"/>
</form>
HTML;
$contents = format_article($title_raw, '(Editing)', $html, NO_WRAP_IN_ARTICLE);
print format_page(
    $title_raw, LOGIN_REDIRECT,
    array(
        'View' =>  "/view/$title_raw",
        'History' => "/history/$title_raw"
    ),
    $contents
);
?>
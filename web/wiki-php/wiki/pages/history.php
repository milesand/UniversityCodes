<?php
if (!isset($_GET['title'])) {
    http_response_code(400);
    exit();
}
if (isset($_GET['until'])) {
    $until = intval($_GET['until']);
} else if (isset($_GET['since'])) {
    $until = intval($_GET['since']) + 14;
} else {
    $until = NULL;
}

$doc_root = rtrim($_SERVER['DOCUMENT_ROOT'], '/');
include_once("$doc_root/snippets/title.php");
include_once("$doc_root/snippets/format.php");
include_once("$doc_root/snippets/db.php");

$title = new Title($_GET['title']);
$title_raw = $title->raw();

$mysqli = new DBConnection();
$stmt = $mysqli->prepare(<<<SQL
    SELECT `page`.`id`, `revision`.`revision_no`
    FROM `page`
    JOIN `revision` ON `revision`.`id` = `page`.`last_rev`
    WHERE `page`.`namespace` = ? AND `page`.`name` = ?
SQL
);
$stmt->bind_param('is', $title->namespace, $title->name);
$stmt->execute();
$stmt->bind_result($page_id, $last_revision_number);
$result = $stmt->fetch();
$stmt->close();
if ($result === NULL) {
    $mysqli->close();
    http_response_code(400);
    exit();
} else if ($result === FALSE) {
    $mysqli->close();
    http_response_code(500);
    exit();
}

if ($last_revision_number < 15) {
    $until = $last_revision_number;
} else if ($until === NULL || $last_revision_number < $until) {
    $until = $last_revision_number;
} else if ($until < 15) {
    $until = 15;
}

$stmt = $mysqli->prepare(<<<SQL
    SELECT
        `revision`.`timestamp`,
        `revision`.`revision_no`,
        `user`.`registered`,
        `user`.`name`
    FROM
    (
        SELECT
            `revision`.`timestamp`,
            `revision`.`revision_no`,
            `revision`.`editor_id`
        FROM
            `revision`
        WHERE `revision`.`page_id` = ?
          AND `revision`.`revision_no` <= ?
        ORDER BY `revision`.`revision_no` DESC
        LIMIT 15
    ) AS `revision`
    JOIN `user` ON `user`.`id` = `revision`.`editor_id`
    ORDER BY `revision`.`revision_no` DESC
SQL
);
$stmt->bind_param('ii', $page_id, $until);
$stmt->execute();
$stmt->bind_result($timestamp, $rev_no, $user_reg, $user_name);
$revisions = array();
$template = <<<HTML
<li>
    <time>%s</time>
    <span>(<a href="/revision/%s/%d">View</a>|<a href="/raw/%s/%d">RAW</a>)</span>
    <strong>r%d</strong>
    <span>by %s</span>
</li>
HTML;
while ($stmt->fetch() === TRUE) {
    if ($user_reg === 0) {
        $user = $user_name;
    } else {
        $user = "<a href=\"/view/User:$user_name\">$user_name</a>";
    }
    $revision = sprintf($template,
        $timestamp,
        $title_raw, $rev_no,
        $title_raw, $rev_no,
        $rev_no,
        $user
    );
    array_push($revisions, $revision);
}
$stmt->close();
$mysqli->close();
$revisions = implode('', $revisions);

$buttons = <<<HTML
<div class="btn-group">
    %s
    %s
</div>
HTML;
if ($until == $last_revision_number) {
    $new = '<span class="btn disabled">New</span><!--';
} else {
    $new_until = $until + 15;
    $new = "<a href=\"?until=$new_until\" class=\"btn\">New</a><!--";
}
if ($until <= 15) {
    $old = '--><span class="btn disabled">Old</span>';
} else {
    $new_until = $until - 15;
    $old = "--><a href=\"?until=$new_until\" class=\"btn\">Old</a>";
}
$buttons = sprintf($buttons, $new, $old);

$contents = <<<HTML
$buttons
$revisions
HTML;

$contents = format_article($title_raw, '(History)', $contents, NO_WRAP_IN_ARTICLE);

print format_page($title_raw, LOGIN_REDIRECT, array('View' => "/view/$title_raw", 'Edit' => "/edit/$title_raw"), $contents);
?>
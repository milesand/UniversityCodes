<?php
mysqli_report(MYSQLI_REPORT_ALL);
if (!isset($_POST['markdown']) || !isset($_GET['title'])) {
    http_response_code(400);
    exit();
};
$doc_root = rtrim($_SERVER['DOCUMENT_ROOT'], '/');
include_once("$doc_root/snippets/db.php");
include_once("$doc_root/snippets/render.php");
include_once("$doc_root/snippets/title.php");
include_once("$doc_root/snippets/encoding.php");

session_start();

$md = ensure_utf8($_POST['markdown']);
$title = new Title($_GET['title']);
$title_raw = $title->raw();

$mysqli = new DBConnection();
$html = render($md, $mysqli);
$mysqli->begin_transaction(MYSQLI_TRANS_START_READ_WRITE);

$stmt = $mysqli->prepare(<<<SQL
    INSERT INTO `text` (`md`, `html`)
    VALUES (?, ?)
SQL
);
$null = NULL;
$stmt->bind_param('bb', $null, $null);
$stmt->send_long_data(0, $md);
$stmt->send_long_data(1, $html);
$stmt->execute();
$stmt->close();
$text_id = $mysqli->insert_id;

if (isset($_SESSION['user_id'])) {
    $user_id = $_SESSION['user_id'];
} else {
    $ip = $_SERVER['REMOTE_ADDR'];
    $stmt = $mysqli->prepare(<<<SQL
        SELECT `user`.`id`
        FROM `user`
        WHERE `user`.`name` = ? AND `user`.`registered` = 0
SQL
    );
    $stmt->bind_param('s', $ip);
    $stmt->bind_result($user_id);
    $stmt->execute();
    $res = $stmt->fetch();
    $stmt->close();
    if ($res !== TRUE) {
        $stmt = $mysqli->prepare(<<<SQL
            INSERT INTO `user` (`name`, `registered`)
            VALUES (?, 0)
SQL
        );
        $stmt->bind_param('s', $ip);
        $stmt->execute();
        $stmt->close();
        $user_id = $mysqli->insert_id;
    }
}

$stmt = $mysqli->prepare(<<<SQL
    SELECT `page`.`id`, `revision`.`revision_no`
    FROM
        `page`
        JOIN `revision` on `revision`.`id` = `page`.`last_rev`
    WHERE `page`.`namespace` = ? AND `page`.`name` = ?
SQL
);
$stmt->bind_param('is', $title->namespace, $title->name);
$stmt->bind_result($page_id, $rev_no);
$stmt->execute();
$res = $stmt->fetch();
$stmt->close();
if ($res === TRUE) {
    $rev_no += 1;
} else {
    // Page does not exist, create it
    // Since there's no entry in table in revision for this edit yet,
    // use a dummy value for `last_rev` field.
    $stmt = $mysqli->prepare(<<<SQL
        INSERT INTO `page` (`namespace`, `name`, `last_rev`, `last_render`)
        VALUES (?, ?, 0, NOW())
SQL
    );
    $stmt->bind_param('is', $title->namespace, $title->name);
    $stmt->execute();
    $stmt->close();
    $page_id = $mysqli->insert_id;
    $rev_no = 1;
}

$stmt = $mysqli->prepare(<<<SQL
    INSERT INTO `revision` (`revision_no`, `page_id`, `text_id`, `timestamp`, `editor_id`)
    VALUES (?, ?, ?, NOW(), ?)
SQL
);
$stmt->bind_param('iiii', $rev_no, $page_id, $text_id, $user_id);
$stmt->execute();
$stmt->close();
$rev_id = $mysqli->insert_id;

$stmt = $mysqli->prepare(<<<SQL
    UPDATE `page`
    SET `last_rev` = ?, `last_render` = NOW()
    WHERE `id` = ?
SQL
);
$stmt->bind_param('ii', $rev_id, $page_id);
$stmt->execute();
$stmt->close();

$mysqli->commit();
$mysqli->close();

http_response_code(301);
header("Location: /view/$title_raw");
exit();
?>
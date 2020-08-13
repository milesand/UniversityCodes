<?php
function update_text($mysqli, $page_id, $text_id, $html) {
    $stmt = $mysqli->prepare('UPDATE `page` SET `page`.`last_render` = ? WHERE `page`.`id` = ?');
    $now = date("Y-m-d H:i:s");
    $stmt->bind_param('si', $now, $page_id);
    $stmt->execute();
    $stmt->close();

    $stmt = $mysqli->prepare('UPDATE `text` SET `text`.`html` = ? WHERE `text`.`id` = ?');
    $null = NULL;
    $stmt->bind_param('bi', $null, $text_id);
    $stmt->send_long_data(0, $html);
    $stmt->execute();
    $stmt->close();
}
?>
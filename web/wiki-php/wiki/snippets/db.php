<?php
$doc_root = rtrim($_SERVER['DOCUMENT_ROOT'], '/');
include_once("$doc_root/snippets/title.php");

class DBConnection extends mysqli {
    function __construct() {
        $doc_root = rtrim($_SERVER['DOCUMENT_ROOT'], '/');
        $json = file_get_contents("$doc_root/setting.json");
        $setting = json_decode($json);
        $db_addr = $setting->db->addr;
        $db_port = $setting->db->port;
        $db_user = $setting->db->user;
        $db_pass = $setting->db->pass;
        $db_name = $setting->db->name;
        parent::__construct($db_addr, $db_user, $db_pass, $db_name, $db_port);
        if ($this->connect_errno) {
            error_log("Failed to connect to MySql: " . $this->connect_error);
            http_response_code(500);
            die();
        }
    }

    function page_exists($title) {
        $title = new Title($title);
        $stmt = $this->prepare('SELECT `page`.`id` FROM `page` WHERE `page`.`namespace` = ? AND `page`.`name` = ? LIMIT 1');
        $stmt->bind_param('is', $title->namespace, $title->name);
        $stmt->execute();
        $stmt->bind_result($_);
        $res = $stmt->fetch();
        $stmt->close();
        return $res;
    }
}
?>

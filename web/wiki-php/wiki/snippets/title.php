<?php
$doc_root = $_SERVER['DOCUMENT_ROOT'];
include_once("$doc_root/snippets/encoding.php");

class Title {
    public $namespace;
    public $namespace_str;
    public $name;

    function __construct($title) {
        $title = ensure_utf8($title);
        $arr = explode(':', $title, 2);
        if (count($arr) == 2) {
            switch ($arr[0]) {
                case '':
                case 'Article':
                    $this->namespace = 0;
                    $this->namespace_str = $arr[0];
                    break;
                case 'Meta':
                    $this->namespace = 1;
                    $this->namespace_str = $arr[0];
                    break;
                case 'User':
                    $this->namespace = 2;
                    $this->namespace_str = $arr[0];
                    break;
                case 'File':
                    $this->namespace = 3;
                    $this->namespace_str = $arr[0];
                    break;
                default:
                    $this->namespace = 0;
                    $this->namespace_str = NULL;
                    $arr[1] = $title;
            }
            $this->name = $arr[1];
        } else {
            $this->namespace = 0;
            $this->namespace_str = NULL;
            $this->name = $title;
        }
        if (strlen($this->name) > 255) {
            $this->name = mb_strimwidth($this->name, 0, 255, '...', 'UTF-8');
        }
    }

    public function raw() {
        if ($this->namespace_str === NULL) {
            return htmlspecialchars($this->name);
        }
        return htmlspecialchars($this->namespace_str . ':' . $this->name);
    }
}
?>
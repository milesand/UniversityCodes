<?php
mb_substitute_character(0xFFFD);
mb_internal_encoding('UTF-8');

function ensure_utf8($str) {
    return mb_convert_encoding($str, 'UTF-8', 'UTF-8');
}
?>
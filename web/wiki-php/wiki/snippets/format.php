<?php
define('LOGIN_REDIRECT', 0);
define('LOGIN_NO_REDIRECT', 1);

define('WRAP_IN_ARTICLE', 0);
define('NO_WRAP_IN_ARTICLE', 1);

function format_page($title, $login_redirect, $article_menu, $contents) {
    $doc_root = rtrim($_SERVER['DOCUMENT_ROOT'], '/');
    $template = file_get_contents("$doc_root/template/page.html");

    $login_status = get_login_status($login_redirect);
    $article_menu = format_article_menu($article_menu);
    
    return sprintf($template, $title, $login_status, $article_menu, $contents);
}

function format_article($title, $title_aux, $contents, $wrap_with_article) {
    $doc_root = rtrim($_SERVER['DOCUMENT_ROOT'], '/');
    $h1 = $title;
    if ($title_aux !== NULL) {
        $title_aux_template = file_get_contents("$doc_root/template/title_aux.html");
        $h1 .= sprintf($title_aux_template, $title_aux);
    }
    $template = file_get_contents("$doc_root/template/article.html");
    $result = sprintf($template, $h1, $contents);
    if ($wrap_with_article === WRAP_IN_ARTICLE) {
        $result = '<article>' . $result . '</article>';
    }
    return $result;
}

function get_login_status($redirect) {
    $doc_root = rtrim($_SERVER['DOCUMENT_ROOT'], '/');
    if (session_status() !== PHP_SESSION_ACTIVE) {
        session_start();
    }
    if (isset($_SESSION['username'])) {
        $template = file_get_contents("$doc_root/template/login_status/logged_in.html");
        $user = $_SESSION['username'];
        $uri = urlencode($_SERVER['REQUEST_URI']);
        return sprintf($template, $user, $user, $uri);
    }
    $template = file_get_contents("$doc_root/template/login_status/not_logged_in.html");
    if ($redirect === LOGIN_REDIRECT) {
        $uri = '/login?redirect=' . urlencode($_SERVER['REQUEST_URI']);
        if ($_SERVER['QUERY_STRING'] !== '') {
            $uri .= '&' . $_SERVER['QUERY_STRING'];
        }
    } else {
        $uri = '/login';
        if ($_SERVER['QUERY_STRING'] !== '') {
            $uri .= '?' . $_SERVER['QUERY_STRING'];
        }
    }
    return sprintf($template, $uri);
}

function format_article_menu($article_menu) {
    if ($article_menu === NULL) {
        return '';
    }
    $doc_root = rtrim($_SERVER['DOCUMENT_ROOT'], '/');
    $template = file_get_contents("$doc_root/template/button.html");
    $arr = array();
    foreach ($article_menu as $text => $path) {
        array_push($arr, sprintf($template, $path, $text));
    }
    return '<div class="article-menu">' . implode('', $arr) . '</div>';
}
?>
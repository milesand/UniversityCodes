<?php
$doc_root = $_SERVER['DOCUMENT_ROOT'];
include_once("$doc_root/snippets/format.php");
session_start();

if (isset($_SESSION['username'])) {
    if (isset($_GET['redirect'])) {
        http_response_code(302);
        $redirect = urldecode($_GET['redirect']);
        header("Location: $redirect");
        exit();
    }
    include("$doc_root/entrance.php");
    exit();
}

$uri = $_SERVER['REQUEST_URI'];
if ($_SERVER['QUERY_STRING'] !== '') {
    $query = '?'.$_SERVER['QUERY_STRING'];
} else {
    $query = '';
}

if (isset($_SESSION['login_fail'])) {
    $optional_fail = "\n<div id=\"login-fail-message\">Login failed. Please try again.</div>";
    unset($_SESSION['login_fail']);
} else {
    $optional_fail = '';
}

$title = 'Log in';
$content = <<<EOF
$optional_fail
<form action="/login/submit$query" method="post" name="login">
    <div id="username_div">
        <label for="username_input">Username</label>
        <input id="username_input" name="username" 
        placeholder="Enter your username" required="">
    </div>
    <button type="submit" value="Log in">Log in</button>
</form>

<div>
<p>Don't have an account? <a class="btn" href="/join$query">Join</a></p>
</div>

EOF;
$content = format_article($title, NULL, $content, NO_WRAP_IN_ARTICLE);

print format_page($title, LOGIN_NO_REDIRECT, NULL, $content);
?>
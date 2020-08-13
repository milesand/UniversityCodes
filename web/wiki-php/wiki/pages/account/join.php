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

if (isset($_SESSION['join_fail'])) {
    $optional_fail = "\n<div id=\"join-fail-message\">Join failed. Please try again.</div>";
    unset($_SESSION['join_fail']);
} else {
    $optional_fail = '';
}

$title = 'Sign up';
$content = <<<EOF
<form action="/join/submit$query" method="post" name="login">$optional_fail
    <div id="username_div">
        <label for="username_input">Username</label>
        <input id="username_input" name="username" 
        placeholder="Enter your username" required="">
    </div>
    <button type="submit" value="join">Join</button>
</form>

<div>
<p>Already have an account? <a class="btn" href="/login$query">Login</a></p>
</div>
EOF;
$content = format_article($title, NULL, $content, NO_WRAP_IN_ARTICLE);

print format_page($title, LOGIN_NO_REDIRECT, NULL, $content);
?>
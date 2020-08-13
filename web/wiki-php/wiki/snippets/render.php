<?php
function render($md, $mysqli) {
    $blocks = blockify($md);
    return $blocks->render($md, $mysqli);
}

function blockify($md) {
    $document = new OpenDocument();

    $len = strlen($md);
    $next_line_start = 0;

    // The main loop. each iteration deals with a single line in input.
    while ($next_line_start < $len) {
        // Find the end of this line and the potential start of next line.
        $text_start = $next_line_start;
        $text_end = $text_start;
        while (TRUE) {
            if ($text_end >= $len) {
                $next_line_start = $text_end + 1;
                break;
            }
            $ch = $md[$text_end];
            // We deal with 3 kinds of different line endings: \n, \r\n, \r.
            if ($ch === "\n") {
                $next_line_start = $text_end + 1;
                break;
            }
            if ($ch === "\r") {
                if ($text_end + 1 < $len && $md[$text_end + 1] === "\n") {
                    $next_line_start = $text_end + 2;
                } else {
                    $next_line_start = $text_end + 1;
                }
                break;
            }
            $text_end += 1;
        }
        // The line of interest lies in $md's [$text_start..$text_end) range.
        // That is, left-inclusive, right-exclusive.
        $document->consume($md, $text_start, $text_end);
    }

    $document->close();

    return $document;
}

function start_block($md, $start, $end) {
    while ($start < $end && $md[$start] === " ") {
        $start += 1;
    }
    if ($start === $end) {
        return NULL;
    }

    $block = Heading::start($md, $start, $end);
    if ($block !== NULL) {
        return $block;
    }
    $block = Blockquote::start($md, $start, $end);
    if ($block !== NULL) {
        return $block;
    }
    $block = UnorderedList::start($md, $start, $end);
    if ($block !== NULL) {
        return $block;
    }
    $block = CodeBlock::start($md, $start, $end);
    if ($block !== NULL) {
        return $block;
    }

    return new Paragraph($start, $end);
}

// Turns out, consume implementations are kind of repetitive.
function consume_boilerplate($md, $start, $end, &$open_block, &$blocks) {
    if ($open_block !== NULL) {
        // Case #1: There's an open block and it's a paragraph.
        // See if the new line opens any new block, and if it does, close paragraph
        // and push the new block.
        // If it doesn't and is not empty, feed it to paragraph.
        // It it is empty, start_block returns NULL.
        if ($open_block::KIND === BlockKind::Paragraph) {
            $block = start_block($md, $start, $end);
            if ($block !== NULL && $block::KIND === BlockKind::Paragraph) {
                $open_block->consume($md, $start, $end);
            } else {
                $open_block->close();
                $blocks[] = $open_block;
                if ($block !== NULL && $block::KIND == BlockKind::MultiLine) {
                    $open_block = $block;
                } else {
                    $open_block = NULL;
                }
            }
            return Consume::Ok;
        }

        // Case #2: There's an open block and it's not a paragraph.
        // We try to feed the line to open block, and if we succeed,
        // We return.
        $result = $open_block->consume($md, $start, $end);
        if ($result !== Consume::Ok) {
            $open_block->close();
            $blocks[] = $open_block;
            $open_block = NULL;
        }
        if ($result !== Consume::Err) {
            return Consume::Ok;
        }
    }
    // There was no open block, or the open block in Case #2 failed to
    // consume the line and was closed.
    // As a result, when we enter this part, the $open_block should be NULL.
    $block = start_block($md, $start, $end);
    if ($block !== NULL) {
        if ($block::KIND === BlockKind::SingleLine) {
            $blocks[] = $block;
        } else {
            $open_block = $block;
        }
    }
    return Consume::Ok;
}

// Kinds of blocks we have.
// Single-line blocks are created and closed at the same line.
// Paragraph is a fall-back block; if the line isn't any other block and not
// empty, it will be considered a paragraph.
// Multi-line blocks, potentially, take up more than a single line.
abstract class BlockKind {
    const SingleLine = 0;
    const Paragraph = 1;
    const MultiLine = 2;
}

// Possible return values from the consume() method.
abstract class Consume {
    const Ok = 0;         // The line has been consumed.
    const OkAndClose = 1; // The line has been consumed, and the block must be closed,
    const Err = 2;        // The line was not consumed.
}

// Just a pair of integer that are indices into given document.
class Span {
    public $start;
    public $end;

    function __construct($start, $end) {
        $this->start = $start;
        $this->end = $end;
    }

    public function apply($md) {
        return substr($md, $this->start, $this->end - $this->start);
    }
}

// The root of the document.
class OpenDocument {
    private $open_block;
    private $blocks;

    function __construct() {
        $this->open_block = NULL;
        $this->blocks = array();
    }

    public function consume($md, $start, $end) {
        consume_boilerplate($md, $start, $end, $this->open_block, $this->blocks);
    }

    public function close() {
        if ($this->open_block !== NULL) {
            $this->open_block->close();
            $this->blocks[] = $this->open_block;
            $this->open_block = NULL;
        }
    }

    public function render($md, $mysqli) {
        $rendered = array();
        foreach ($this->blocks as $block) {
            $rendered[] = $block->render($md, $mysqli);
        }
        return implode('', $rendered);
    }
}

// The block elements.
//
// the KIND constant is one of the BLOCK_KIND constants.
// when the block_kind returns non-BLOCK_KIND_SINGLE_LINE kind,
// then that block has some more relevant methods:
//   close():
//     Close the block.
//   consume($md, $start, $end):
//     Try to consume the text. On success, return TRUE; FALSE otherwise.

class Heading {
    private $level;
    private $inline_span;

    const KIND = BlockKind::SingleLine;

    function __construct($level, $inline_start, $inline_end) {
        $this->level = $level;
        $this->inline_span = new Span($inline_start, $inline_end);
    }

    public static function start($md, $start, $end) {
        if ($end - $start < 6 || substr($md, $start, 2) !== "==" || substr($md, $end - 2, 2) !== "==") {
            return NULL;
        }
        $level = 2;
        $start += 2;
        while ($start < $end && $md[$start] === "=") {
            $level += 1;
            if ($level > 6) {
                return NULL;
            }
            $start += 1;
        }
        if ($start === $end || $md[$start] !== " ") {
            return NULL;
        }
        $inline_start = $start + 1;
        $inline_end = $end - $level - 1;
        if ($inline_end <= $inline_start || $md[$inline_end] !== " ") {
            return NULL;
        }
        $idx = $inline_end + 1;
        while ($idx < $end - 2) {
            if ($md[$idx] !== "=") {
                return NULL;
            }
            $idx += 1;
        }
        return new Heading($level, $inline_start, $inline_end);
    }

    public function render($md, $mysqli) {
        $level = $this->level;
        $text = $this->inline_span->apply($md);
        $rendered = render_inline($text, $mysqli);
        return "<h$level>$rendered</h$level>";
    }
}

class Paragraph {
    private $spans;
    
    const KIND = BlockKind::Paragraph;

    function __construct($start, $end) {
        $this->spans = array(new Span($start, $end));
    }

    // Paragraph just consume what was given, without checking what it is.
    // Parent blocks should take caution.
    public function consume($md, $start, $end) {
        $this->spans[] = new Span($start, $end);
    }

    public function close() {
        // no-op
    }

    public function render($md, $mysqli) {
        $text_arr = array();
        foreach ($this->spans as $span) {
            $text_arr[] = $span->apply($md);
        }
        $text = implode("\n", $text_arr);
        $rendered = render_inline($text, $mysqli);
        return "<p>$rendered</p>";
    }
}

class Blockquote {
    private $open_block;
    private $blocks;

    const KIND = BlockKind::MultiLine;

    function __construct($md, $start, $end) {
        $this->open_block = NULL;
        $this->blocks = array();
        $item = start_block($md, $start + 2, $end);
        if ($item !== NULL) {
            if ($item::KIND === BlockKind::SingleLine) {
                $this->blocks[] = $item;
            } else {
                $this->open_block = $item;
            }
        }
    }

    public static function start($md, $start, $end) {
        if ($start + 1 >= $end || substr($md, $start, 2) !== "> ") {
            return NULL;
        }
        return new Blockquote($md, $start, $end);
    }

    public function consume($md, $start, $end) {
        if ($end - $start < 2 || substr($md, $start, 2) !== "> ") {
            return Consume::Err;
        }
        $start += 2;
        consume_boilerplate($md, $start, $end, $this->open_block, $this->blocks);
        return Consume::Ok;
    }

    public function close() {
        if ($this->open_block !== NULL) {
            $this->open_block->close();
            $this->blocks[] = $this->open_block;
            $this->open_block = NULL;
        }
    }

    public function render($md, $mysqli) {
        $rendered = array();
        foreach ($this->blocks as $block) {
            $rendered[] = $block->render($md, $mysqli);
        }
        return '<blockquote>' . implode('', $rendered) . '</blockquote>';
    }
}

class UnorderedList {
    // All child blocks of List blocks are ListItem. 
    private $bullet;
    private $open_block;
    private $blocks;

    const KIND = BlockKind::MultiLine;

    function __construct($md, $start, $end) {
        $this->bullet = $md[$start];
        $this->blocks = array();
        $this->open_block = new ListItem();
        $result = $this->open_block->consume($md, $start + 2, $end);
        if ($result !== Consume::Ok) {
            $this->open_block->close();
            $this->blocks[] = $this->open_block;
            $this->open_block = NULL;
        }
    }

    public static function start($md, $start, $end) {
        if ($start + 1 >= $end && $md[$start + 1] !== " ") {
            return NULL;
        }
        $bullet = $md[$start];
        if ($bullet !== "-" && $bullet !== "*") {
            return NULL;
        }
        return new UnorderedList($md, $start, $end);
    }

    public function consume($md, $start, $end) {
        if ($end - $start < 2 || $md[$start + 1] !== " ") {
            return Consume::Err;
        }
        if ($md[$start] === $this->bullet) {
            $this->open_block->close();
            $this->blocks[] = $this->open_block;
            $this->open_block = new ListItem();
            $this->open_block->consume($md, $start + 2, $end);
        } else if ($md[$start] === " ") {
            $this->open_block->consume($md, $start + 2, $end);
        } else {
            return Consume::Err;
        }
        return Consume::Ok;
    }

    public function close() {
        if ($this->open_block !== NULL) {
            $this->open_block->close();
            $this->blocks[] = $this->open_block;
            $this->open_block = NULL;
        }
    }

    public function render($md, $mysqli) {
        $rendered = array();
        foreach ($this->blocks as $block) {
            $rendered[] = $block->render($md, $mysqli);
        }
        return '<ul>' . implode('', $rendered) . '</ul>';
    }
}

class ListItem {
    private $open_block;
    private $blocks;

    function __construct() {
        $this->open_block = NULL;
        $this->blocks = array();
    }

    public function consume($md, $start, $end) {
        return consume_boilerplate($md, $start, $end, $this->open_block, $this->blocks);
    }

    public function close() {
        if ($this->open_block !== NULL) {
            $this->open_block->close();
            $this->blocks[] = $this->open_block;
            $this->open_block = NULL;
        }
    }

    public function render($md, $mysqli) {
        $rendered = array();
        foreach ($this->blocks as $block) {
            $rendered[] = $block->render($md, $mysqli);
        }
        return '<li>' . implode('', $rendered) . '</li>';
    }
}

class CodeBlock {
    private $fence_len;
    private $spans;

    const KIND = BlockKind::MultiLine;

    function __construct($fence_len) {
        $this->fence_len = $fence_len;
        $this->spans = array();
    }

    public static function start($md, $start, $end) {
        if ($end - $start >= 3) {
            for ($idx = $start; $idx < $end; $idx += 1) {
                if ($md[$idx] !== "`") {
                    return NULL;
                }
            }
            return new CodeBlock($end - $start);
        }
        return NULL;
    }

    public function consume($md, $start, $end) {
        if ($end - $start === $this->fence_len) {
            for ($idx = $start; $idx < $end; $idx += 1) {
                if ($md[$idx] !== "`") {
                    $this->spans[] = new Span($start, $end);
                    return Consume::Ok;
                }
            }
            return Consume::OkAndClose;
        }
        $this->spans[] = new Span($start, $end);
        return Consume::Ok;
    }
    
    public function close() {
        // No-op
    }

    public function render($md, $mysqli) {
        $text_arr = array();
        foreach ($this->spans as $span) {
            $text_arr[] = $span->apply($md);
        }
        return '<pre class="code"><code>' . htmlspecialchars(implode("\n", $text_arr)) . '</code></pre>';
    }
}

function render_inline($text, $mysqli) {
    $text = htmlspecialchars($text);
    $text = preg_replace("/'''''(.*)'''''/", '<strong><em>$1</em></strong>', $text);
    $text = preg_replace("/'''(.*)'''/", '<strong>$1</strong>', $text);
    $text = preg_replace("/''(.*)''/", '<em>$1</em>', $text);
    $text = preg_replace("/~~(.*)~~/", '<del>$1</del>', $text);

    $make_internal_link = function($link, $show) use ($mysqli) {
        $res = $mysqli->page_exists($link);
        if ($res === TRUE) {
            return "<a href=\"/view/$link\" class=\"valid-internal-link\">$show</a>";
        } else if ($res === NULL) {
            return "<a href=\"/view/$link\" class=\"invalid-internal-link\">$show</a>";
        } else { // $res === FALSE
            http_response_code(500);
            die();
        }
    };

    $text = preg_replace_callback('/\[\[(.*)\|(.*)\]\]/', 
        function ($match) use ($make_internal_link) {
            return ($make_internal_link)($match[1], $match[2]);
        },
        $text
    );
    
    $text = preg_replace_callback('/\[\[(.*)\]\]/',
        function ($match) use ($make_internal_link) {
            return ($make_internal_link)($match[1], $match[1]);
        },
        $text
    );

    $text = preg_replace('/\[([^ ]*)\]/', '<a href="$1" class="external-link">$1</a>', $text);
    $text = preg_replace('/\[([^ ]*) (.*)\]/', '<a href="$1" class="external-link">$2</a>', $text);
    return $text;
}
?>
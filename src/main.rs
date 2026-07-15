use reedline::{DefaultPrompt, Reedline, Signal};

fn main() {
    let mut line_editor = Reedline::create();
    let prompt = DefaultPrompt::default();

    let builtins: Vec<&str> = vec![
        ".",
        ":",
        "[",
        "alias",
        "and",
        "begin",
        "bg",
        "bind",
        "block",
        "break",
        "breakpoint",
        "builtin",
        "caller",
        "case",
        "cd",
        "command",
        "commandline",
        "compgen",
        "complete",
        "compopt",
        "contains",
        "continue",
        "count",
        "declare",
        "dirs",
        "disown",
        "echo",
        "else",
        "emit",
        "enable",
        "end",
        "eval",
        "exec",
        "exit",
        "export",
        "false",
        "fc",
        "fg",
        "for",
        "function",
        "funced",
        "funcsave",
        "functions",
        "getopts",
        "hash",
        "help",
        "history",
        "if",
        "isatty",
        "jobs",
        "kill",
        "let",
        "local",
        "logout",
        "mapfile",
        "math",
        "newgrp",
        "not",
        "or",
        "popd",
        "printf",
        "pushd",
        "pwd",
        "random",
        "read",
        "readarray",
        "readonly",
        "realpath",
        "return",
        "select",
        "set",
        "set_color",
        "shift",
        "shopt",
        "source",
        "status",
        "string",
        "string-collect",
        "string-escape",
        "string-join",
        "string-join0",
        "string-length",
        "string-lower",
        "string-match",
        "string-pad",
        "string-repeat",
        "string-replace",
        "string-shorten",
        "string-split",
        "string-split0",
        "string-sub",
        "string-trim",
        "string-unescape",
        "string-upper",
        "suspend",
        "switch",
        "test",
        "time",
        "times",
        "trap",
        "true",
        "type",
        "typeset",
        "ulimit",
        "umask",
        "unalias",
        "unset",
        "until",
        "vared",
        "variables",
        "wait",
        "while",
        "wave",
        "wave_add_path",
        "wave_breakpoint_prompt",
        "wave_clipboard_copy",
        "wave_clipboard_paste",
        "wave_command_not_found",
        "wave_config",
        "wave_default_key_bindings",
        "wave_git_prompt",
        "wave_greeting",
        "wave_indent",
        "wave_is_root_user",
        "wave_key_reader",
        "wave_mode_prompt",
        "wave_opt",
        "wave_prompt",
        "wave_right_prompt",
        "wave_title",
        "wave_update_completions",
        "wave_vcs_prompt",
    ];

    loop {
        let ugc = line_editor.read_line(&prompt);

        match ugc {
            Ok(Signal::Success(buffer)) => {
                let mut splitted_command = buffer.trim().split_whitespace();

                if let Some(command) = splitted_command.next() {
                    let args: Vec<&str> = splitted_command.collect();

                    let builtin_cmd = builtins.binary_search(&command).is_ok();

                    if builtin_cmd {
                        println!("Executing builtin: {command} with args: {args:?}");
                    } else {
                        println!("Executing external command: {command} with args: {args:?}");
                    }
                }
            }
            Ok(Signal::CtrlD) | Ok(Signal::CtrlC) => {
                println!("\nAborted!");
                break;
            }

            x => {
                println!("Event: {:?}", x);
            }
        }
    }
}

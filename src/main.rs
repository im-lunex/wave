use reedline::{DefaultPrompt, Reedline, Signal};

fn main() {
    let mut line_editor = Reedline::create();
    let prompt = DefaultPrompt::default();

    loop {
        let ugc = line_editor.read_line(&prompt);
        match ugc {
            Ok(Signal::Success(buffer)) => match buffer.trim() {
                "lunex" => {
                    println!("Goat")
                }
                _other => {
                    println!("NIGH")
                }
            },
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

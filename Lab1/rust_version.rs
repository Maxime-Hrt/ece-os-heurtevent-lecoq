use std::io::{self, Write};
use std::process::{Command, exit};

fn my_system(command: &str) -> i32 {
    if command.is_empty() {
        return -1;
    }

    let output = Command::new("sh")
        .arg("-c")
        .arg(command)
        .output();

    match output {
        Ok(output) => {
            if output.status.success() {
                //Print output
                io::stdout().write_all(&output.stdout).expect("Failed to write to stdout");
                io::stderr().write_all(&output.stderr).expect("Failed to write to stderr");
                return output.status.code().unwrap_or(0);
            } else {
                eprintln!("Command failed: {:?}", output.status);
                return -4;
            }
        }
        Err(e) => {
            eprintln!("Error executing command: {}", e);
            return -2;
        }
    }
}

fn main() {
    let mut choice = String::new();
    let mut command = String::new();

    loop {
        // Display menu
        println!("\n---Menu---");
        println!("1. Run program");
        println!("2. Kill a process");
        println!("3. List files in the current folder");
        println!("4. Open application (On MacOS)");
        println!("5. Exit");

        print!("Enter your choice: ");
        io::stdout().flush().expect("Failed to flush");

        io::stdin().read_line(&mut choice).expect("Failed to read line");
        choice = choice.trim().to_string();

        match choice.as_str() {
            "1" => {
                print!("Enter the command name to run: ");
                io::stdout().flush().expect("Failed to flush");
                io::stdin().read_line(&mut command).expect("Failed to read line");
                command = command.trim().to_string();
                let result = my_system(&command);
                println!("Command returned: {}", result);
            }
            "2" => {
                print!("Enter the process ID to kill: ");
                io::stdout().flush().expect("Failed to flush");
                let mut pid = String::new();
                io::stdin().read_line(&mut pid).expect("Failed to read line");
                pid = pid.trim().to_string();
                let command = format!("kill {}", pid);
                let result = my_system(&command);
                println!("Command returned: {}", result);
            }
            "3" => {
                let result = my_system("ls");
                println!("Command returned: {}", result);
            }
            "4" => {
                print!("Enter the application name to open: ");
                io::stdout().flush().expect("Failed to flush");
                io::stdin().read_line(&mut command).expect("Failed to read line");
                command = command.trim().to_string();
                let command = format!("/Applications/Figma.app/Contents/MacOS/{}", command);
                let result = my_system(&command);
                println!("Command returned: {}", result);
            }
            "5" => {
                println!("Exiting...");
                exit(0);
            }
            _ => println!("Invalid choice, try again"),
        }
        choice.clear();
        command.clear();
    }
}

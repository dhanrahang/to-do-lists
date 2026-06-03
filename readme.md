# Task Tracker CLI

A simple command-line task tracker written in C++. It lets you add, update, delete, mark, and list tasks from the terminal. Tasks are saved in a local JSON file named `tasks.json`.

## Features

- Add new tasks
- Update task descriptions
- Delete tasks
- Mark tasks as `in-progress`
- Mark tasks as `done`
- List all tasks
- Filter tasks by status: `todo`, `in-progress`, or `done`

## Task Properties

Each task has:

- `id`
- `description`
- `status`
- `createdAt`
- `updatedAt`

## Requirements

- C++17 or newer
- A C++ compiler such as `g++`

No external libraries are required.

## Build

From the project folder, run:

```bash
g++ -std=c++17 main.cpp -o task-cli.exe
```

## Usage

Add a task:

```bash
.\task-cli.exe add "Buy groceries"
```

List all tasks:

```bash
.\task-cli.exe list
```

Update a task:

```bash
.\task-cli.exe update 1 "Buy groceries and cook dinner"
```

Delete a task:

```bash
.\task-cli.exe delete 1
```

Mark a task as in progress:

```bash
.\task-cli.exe mark-in-progress 1
```

Mark a task as done:

```bash
.\task-cli.exe mark-done 1
```

List tasks by status:

```bash
.\task-cli.exe list todo
.\task-cli.exe list in-progress
.\task-cli.exe list done
```

## Data Storage

The app stores tasks in `tasks.json` in the current directory. If the file does not exist, the program creates it automatically.

## Example

```bash
.\task-cli.exe add "Learn C++ file handling"
.\task-cli.exe mark-in-progress 1
.\task-cli.exe list in-progress
```

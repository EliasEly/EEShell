# SHELL

__Author : Elias El Yandouzi__

__Date : October 2020__

__State : Can be improved__

## About the shell

This shell is written in C and can execute builtin command and program referenced in the PATH variable.

## Build and Run

```bash
$> make
$> ./shell
```

To leave the process, a EOF signal must be send (_^D_ does the job for instance).


## Builtin Command

Mainly through system call in C. Nothing very tricky. No fork here.

_NB: __(OLDPWD__, __PWD)__ and __cd__ are independant._

## Fork, wait and execve

Fork will duplicate the process in the exact same state.

Wait will halt the parent process until the child is terminated. It cleans also all the ressources allocated to child process. If we don't call wait, we have ZOMBIE.

The execve function will replace the current running process with a new one.

## Pipe and dup2

The shell can run command like so :

    ls -al | cat | grep .git

_Useful links:_ 
 - [Why should we close the pipe descriptors](https://stackoverflow.com/questions/33884291/pipes-dup2-and-exec)
 - [How the pipe works](http://www.zeitoun.net/articles/communication-par-tuyau/start)
 - [Useful answers about fd and fork](https://unix.stackexchange.com/questions/91058/file-descriptor-and-fork)

## Redirection

The redirection can be done in the two ways.

Ouput redirected :
- It can just create a new file or remplace its content if it already exists :

        ls -al > toto
- It can append the output to the end of the file :

        ls -al | grep .gitignore >> titi

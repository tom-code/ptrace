#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <fcntl.h>

#include <sys/reg.h>
#include <unistd.h>
#include <stdio.h>
#include <vector>
#include <string>

extern std::vector<std::string> get_syscall_name_vector();

void demo_function() {
  getpid();
  int f = open("pt.cc", O_RDONLY);
  lseek(f, 10, SEEK_SET);
  close(f);
  getpid();
  execl("/bin/ls", "/bin/ls", nullptr);
}

int main()
{
  pid_t child;

  auto syscall_names = get_syscall_name_vector();

  child = fork();
  if (child == 0) {  //forked child
    ptrace(PTRACE_TRACEME, 0, NULL, NULL);
    kill(getpid(), SIGINT);
    demo_function();
  } else {  //parent
      int err;
      struct user_regs_struct uregs;

      wait(NULL);
      while (1) {
        err = ptrace(PTRACE_SYSCALL, child, 0, 0); // wait for syscall entry
        if (err < 0) break;
        wait(NULL);

        err = ptrace(PTRACE_GETREGS, child, 0, &uregs);
        printf("detected syscall name=%s first_arg=%llx\n", syscall_names[uregs.orig_rax].c_str(), uregs.rdi);

        err = ptrace(PTRACE_SYSCALL, child, 0, 0); // wait for syscall return
        wait(NULL);        
      }
  }
}



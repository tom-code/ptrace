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
  execl("/bin/ls", "/bin/ls", nullptr);
}

int main()
{
  auto syscall_names = get_syscall_name_vector();

  pid_t child = fork();
  if (child == 0) {  //forked child
    ptrace(PTRACE_TRACEME, 0, NULL, NULL);
    kill(getpid(), SIGINT);
    demo_function();
  } else {  //parent
      wait(NULL);   // wait for initial signal

      ptrace(PTRACE_SYSCALL, child, 0, 0);
      wait(NULL);

      while (1) {
        int err = ptrace(PTRACE_SYSCALL, child, 0, 0); // wait for syscall entry
        if (err < 0) break;
        wait(NULL);

        struct user_regs_struct uregs;
        err = ptrace(PTRACE_GETREGS, child, 0, &uregs);
        if (err < 0) break;
        if (uregs.orig_rax == 59) { // special handling for exec
          printf(">>syscall exec handled\n");
          err = ptrace(PTRACE_SYSCALL, child, 0, 0);
          wait(NULL);
          err = ptrace(PTRACE_SYSCALL, child, 0, 0);
          wait(NULL);
          continue;
        }
        printf(">>syscall name=%s    first_arg=0x%llx", syscall_names[uregs.orig_rax].c_str(), uregs.rdi);

        err = ptrace(PTRACE_SYSCALL, child, 0, 0); // wait for syscall return
        if (err < 0) break;

        err = ptrace(PTRACE_GETREGS, child, 0, &uregs);
        printf("  return=0x%llx\n", uregs.rax);
        wait(NULL);
      }
  }
}



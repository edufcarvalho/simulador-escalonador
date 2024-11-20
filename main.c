#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

enum IOType {
  FINISHED,            // 0
  HIGH_PRIORITY_QUEUE, // 1
  LOW_PRIORITY_QUEUE,  // 2
  DISK,                // 3
  TAPE,                // 4
  PRINTER,             // 5
};

typedef struct {
  int *self;
  int length;
} List;

typedef struct {
  int arrival;
  enum IOType type;
  int elapse_time;
  int PID;
} IO;

typedef struct {
  IO *self;
  int length;
} IOQueue;

typedef struct {
  int PID;
  int arrival;
  int total_time;
  int elapse_time;
  IOQueue io;
} Process;

typedef struct {
  Process *self;
  int length;
} ProcessQueue;

ProcessQueue high_priority,
  low_priority,
  blocked,
  finished;

IOQueue disk,
  tape,
  printer;

// Functions declaration
List split(char *str);
void *deleteElementFromQueue(void *queue, void *element);
ProcessQueue read_file(char *path);
Process *execute_disk_queue();
Process *execute_tape_queue();
Process *execute_printer_queue();
Process *search_blocked(int PID);
void remove_io(IOQueue *queue, int PID);
void remove_process(ProcessQueue *queue, int PID);

ProcessQueue read_file(char *path) {
  static int PID = 0;
  int k = 0;

  FILE *file = fopen(path, "r");

  if (file == NULL) {
    perror("Error while opening file");
    exit(1);
  }

  ProcessQueue processes;
  processes.self = malloc(100 * sizeof(Process));
  processes.length = 0;

  char line[1000];

  while (fgets(line, sizeof(line), file)) {
    List input = split(line);
    int length = sizeof(input)/sizeof(int),
      io_count = length - 2;

    Process new_process;
    new_process.PID = PID;
    new_process.arrival = input.self[0];
    new_process.total_time = input.self[1];
    new_process.elapse_time = 0;
    new_process.io.self = malloc(io_count * sizeof(IO));
    new_process.io.length = io_count;
    processes.self[processes.length++] = new_process;

    for (int i = 2, j = 0; i < length + 1; i += 2) {
      IO new_io;
      new_io.PID = PID;
      new_io.arrival = input.self[i];
      new_io.elapse_time = 0;
      new_io.type = input.self[i + 1];
      new_process.io.self[j++] = new_io;
    }
  }

  PID += 1;

  return processes;
}

Process *execute_disk_queue() {
  IO *io = disk.self;
  io->elapse_time += 1;
  
  if (io->elapse_time >= DISK) {
    Process *blocked = search_blocked(io->PID), *persist;
    persist = memcpy(&persist, &blocked, sizeof(IO));
    remove_io(&disk, io->PID);
    low_priority.self[low_priority.length++] = *persist;
    return persist;
  }
}

Process *execute_tape_queue() {
  IO *io = tape.self;
  io->elapse_time += 1;
  
  if (io->elapse_time >= TAPE) {
    Process *blocked = search_blocked(io->PID), *persist;
    persist = memcpy(&persist, &blocked, sizeof(IO));
    remove_io(&tape, io->PID);
    high_priority.self[high_priority.length++] = *persist;
    return persist;
  }
}

Process *execute_printer_queue() {
  IO *io = printer.self;
  io->elapse_time += 1;
  
  if (io->elapse_time >= PRINTER) {
    Process *blocked = search_blocked(io->PID), *persist;
    persist = memcpy(&persist, &blocked, sizeof(IO));
    remove_io(&printer, io->PID);
    high_priority.self[high_priority.length++] = *persist;
    return persist;
  }
}

void execute_low_priority_queue() {
  Process *process = low_priority.self;
  process->elapse_time += 1;

  if (process->io.length > 0 && process->elapse_time >= process->io.self[0].arrival) {
    if (process->io.self[0].type == DISK)
      disk.self[disk.length++] = process->io.self[0];
    if (process->io.self[0].type == TAPE)
      disk.self[disk.length++] = process->io.self[0];
    if (process->io.self[0].type == PRINTER)
      disk.self[disk.length++] = process->io.self[0];

    blocked.self[blocked.length++] = *process;
    remove_process(&low_priority, process->PID);
    return;
  }

  if (process->total_time <= process->elapse_time) {
    finished.self[finished.length++] = *process;
    remove_process(&low_priority, process->PID);
    return;
  }

  low_priority.self[low_priority.length++] = *process;
  remove_process(&low_priority, process->PID);
}

void execute_high_priority_queue() {
  Process *process = high_priority.self;
  process->elapse_time += 1;

  if (process->io.length > 0 && process->elapse_time >= process->io.self[0].arrival) {
    if (process->io.self[0].type == DISK)
      disk.self[disk.length++] = process->io.self[0];
    if (process->io.self[0].type == TAPE)
      disk.self[disk.length++] = process->io.self[0];
    if (process->io.self[0].type == PRINTER)
      disk.self[disk.length++] = process->io.self[0];

    blocked.self[blocked.length++] = *process;
    remove_process(&high_priority, process->PID);
    return;
  }

  if (process->total_time <= process->elapse_time) {
    finished.self[finished.length++] = *process;
    remove_process(&high_priority, process->PID);
    return;
  }

  high_priority.self[high_priority.length++] = *process;
  remove_process(&high_priority, process->PID);
}

void execute_processes() {
  Process disk_process, tape_process, printer_process;

  if (disk.length > 0) {
    Process *disk_process = execute_disk_queue();
  }
  if (tape.length > 0) {
    Process *tape_process = execute_tape_queue();
  }
  if (printer.length > 0) {
    Process *printer_process = execute_printer_queue(); 
  }

  if (high_priority.length > 0 && high_priority.self[0].PID != disk_process.PID && high_priority.self[0].PID != tape_process.PID && high_priority.self[0].PID != printer_process.PID) {
    execute_high_priority_queue();
  } else if (low_priority.length > 0 && low_priority.self[0].PID != disk_process.PID && low_priority.self[0].PID != tape_process.PID && low_priority.self[0].PID != printer_process.PID) {
    execute_low_priority_queue();
  }

  return;
}

List split(char *str) {
  List ret;
  int length = strlen(str)/2, n = 0;
  char *token;

  ret.self = malloc(sizeof(int) * length);
  // Split string by whitespace and convert to integer
  token = strtok(str, " ");
  while (token != NULL) {
    ret.self[n++] = atoi(token);
    token = strtok(NULL, " ");
  }

  ret.length = length;
  return ret;
}


Process *search_blocked(int PID) {
  Process *found;

  for (int i = 0; i < blocked.length; i++) {
    if (blocked.self[i].PID == PID) {
      memcpy(&found, &blocked.self[i], sizeof(IO));
      // Erases element
      for (int j = i; j < blocked.length - 1; j++) {
        blocked.self[j] = blocked.self[j + 1];
      }
      // Adjust queue size
      blocked.length--;
      return found;
    }
  }

  return NULL;
}

void remove_io(IOQueue *queue, int PID) {
  if (PID == NULL)
    return;

  for (int i = 0; i < queue->length; i++) {
    if (queue->self[i].PID == PID) {
      // Erases element
      for (int j = i; j < queue->length - 1; j++) {
        queue->self[j] = queue->self[j + 1];
      }
      // Adjust queue size
      queue->length--;
      return;
    }
  }
}

void remove_process(ProcessQueue *queue, int PID) {
  if (PID == NULL)
    return;

  for (int i = 0; i < queue->length; i++) {
    if (queue->self[i].PID == PID) {
      // Erases element
      for (int j = i; j < queue->length - 1; j++) {
        queue->self[j] = queue->self[j + 1];
      }
      // Adjust queue size
      queue->length--;
      return;
    }
  }
}

void process_entry(ProcessQueue *processes, int current_time) {
  for (int i = 0; i < processes->length; i++) {
    if (processes->self[i].arrival <= current_time) {
      high_priority.self[high_priority.length++] = processes->self[i];
      remove_process(processes, processes->self[i].PID);
    }
  }

  return;
}

void print_pids_io(const char *title, IOQueue *processes) {
  printf("%s\n", title);

  for (int i = 0; i < processes->length; i++) {
    printf("[%5d] ", processes->self[i].PID);
  }

  printf("\n");
  return;
}

void print_pids(const char *title, ProcessQueue *processes) {
  printf("%s\n", title);

  for (int i = 0; i < processes->length; i++) {
    printf("[%5d] ", processes->self[i].PID);
  }

  printf("\n");
  return;
}

int main() {
  ProcessQueue processes = read_file("processos.txt");

  for (int i = 0; i < processes.length; i++) {
    printf("%d %d ", processes.self[i].arrival, processes.self[i].total_time);

    for (int j = 0; j < processes.self[i].io.length; j++) {
      printf("%d %d ", processes.self[i].io.self[j].arrival, processes.self[i].io.self[j].type);
    }

    printf("\n");
  }

  high_priority.self = malloc(100 * sizeof(Process));
  high_priority.length = 0;

  low_priority.self = malloc(100 * sizeof(Process));
  low_priority.length = 0;

  disk.self = malloc(100 * sizeof(IO));
  disk.length = 0;

  tape.self = malloc(100 * sizeof(IO));
  tape.length = 0;

  printer.self = malloc(100 * sizeof(IO));
  printer.length = 0;

  blocked.self = malloc(100 * sizeof(Process));
  blocked.length = 0;

  finished.self = malloc(100 * sizeof(Process));
  finished.length = 0;

  int timer = 0;

  while (1) {
    process_entry(&processes, timer);
    printf("\nTEMPO = %d\n", timer);

    print_pids("Fila Finalizados", &finished);
    print_pids("Fila Alta Prioridade", &high_priority);
    print_pids("Fila Baixa Prioridade", &low_priority);
    print_pids_io("Fila Disco", &disk);
    print_pids_io("Fila Fita", &tape);
    print_pids_io("Fila Impressora", &printer);
    print_pids("Fila Bloqueados", &blocked);
    timer += 1;
    sleep(100);
  }
  return 0;
}
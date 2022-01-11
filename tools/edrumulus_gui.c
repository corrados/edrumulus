
// Edrumulus simple terminal GUI
// compile with: gcc edrumulus_gui.c -o gui -lncurses

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <curses.h>

// utility function to get current MIDI command
unsigned char* get_midi_cmd ( int cmd, int val )
{
  static unsigned char midi_cmd[3];
  midi_cmd[0] = 185; // control change MIDI message on channel 10
  midi_cmd[1] = cmd;
  midi_cmd[2] = val;
  return midi_cmd;
}

// main function
int main()
{
  WINDOW* mainwin;

  int ch;
  int serial_port;
  struct termios tty;

  // open serial USB port and set correct baud rate
  serial_port = open("/dev/ttyUSB0", O_RDWR);

  if ( tcgetattr ( serial_port, &tty ) != 0 )
  {
    fprintf ( stderr, "Error from tcgetattr.\n" );
    exit ( EXIT_FAILURE );
  }

  cfsetispeed ( &tty, B38400 );
  cfsetospeed ( &tty, B38400 );

  if ( tcsetattr ( serial_port, TCSANOW, &tty ) != 0 )
  {
    fprintf ( stderr, "Error from tcsetattr.\n" );
    exit ( EXIT_FAILURE );
  }


  // initialize GUI
  mainwin = initscr();
  noecho();                 // turn off key echoing
  keypad ( mainwin, true ); // enable the keypad for non-char keys

  // show usage
  mvaddstr ( 5, 10, "Press a key, q:quit, p:sel pad, a:MIDI curve" );
  refresh();
  move(7, 10);

  // loop until user presses q
  while ( ( ch = getch() ) != 'q' )
  {
    // delete the old response line, and print a new one
    deleteln();

/* TEST */
if ( ch == 'p' )
{
  mvprintw ( 7, 10, "p:sel pad" );
  write(serial_port, get_midi_cmd ( 108, 0 ), 3);
}
if ( ch == 'a' )
{
  mvprintw ( 7, 10, "a:MIDI curve" );
  unsigned char midi_cmd[] = "\xB9\x6D\x03";
  write(serial_port, midi_cmd, 3);
}

    refresh();
  }

  // clean up and exit
  delwin ( mainwin );
  endwin();
  refresh();
  close ( serial_port );
  return EXIT_SUCCESS;
}


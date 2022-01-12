
// Edrumulus simple terminal GUI
// compile with: gcc edrumulus_gui.cpp -o gui -lncurses

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <algorithm>
#include <termios.h>
#include <curses.h>

// tables
const int   max_num_pads = 8;
const int   number_cmd   = 12;
const char* cmd_names[]  = { "type", "thresh", "sens", "pos thres", "pos sens", "rim thres", "curve", "spike", "rim/pos", "note", "note rim", "cross" };
const int   cmd_val[]    = {    102,      103,    104,         105,        106,         107,     109,     110,       111,    112,        113,     114 };

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
  int            ch;
  struct termios tty;

  // open serial USB port and set correct baud rate
  int serial_port = open ( "/dev/ttyUSB0", O_RDWR | O_NONBLOCK );
  int ret         = tcgetattr ( serial_port, &tty );
  cfsetispeed ( &tty, B38400 );
  cfsetospeed ( &tty, B38400 );
  if ( tcsetattr ( serial_port, TCSANOW, &tty ) != 0 || ret != 0 )
  {
    fprintf ( stderr, "Is Edrumulus connected? Are you in dialout group (sudo usermod -a -G dialout $USER)?\n" );
    exit ( EXIT_FAILURE );
  }

  // initialize GUI
  WINDOW* mainwin = initscr();
  noecho();                 // turn off key echoing
  keypad ( mainwin, true ); // enable the keypad for non-char keys
  int sel_pad = 0;
  int sel_cmd = 0;
  nodelay ( mainwin, true ); // we want a non-blocking getch()

  // show usage
  mvaddstr ( 5, 10, "Press a key, q:quit, s,S:sel pad, c,C:sel command" );
  refresh();

  // loop until user presses q
  while ( ( ch = getch() ) != 'q' )
  {
    if ( ch != -1 )
    {
      // delete the old response lines
      move ( 8, 10 ); deleteln();
      move ( 7, 10 ); deleteln();
      move ( 6, 10 ); deleteln();

      if ( ch == 's' || ch == 'S' ) // change selected pad
     {
       ch == 's' ? sel_pad++ : sel_pad--;
       sel_pad = std::max ( 0, std::min ( max_num_pads - 1, sel_pad ) );
       write ( serial_port, get_midi_cmd ( 108, sel_pad ), 3 );
     }

      if ( ch == 'c' || ch == 'C' ) // change selected command
     {
       ch == 'c' ? sel_cmd++ : sel_cmd--;
       sel_cmd = std::max ( 0, std::min ( number_cmd - 1, sel_cmd ) );
     }

      mvprintw ( 7, 10, "Selected pad: %d",     sel_pad );
      mvprintw ( 6, 10, "Selected command: %s", cmd_names[sel_cmd] );
      refresh();
    }

/*
unsigned char read_buf[3];
int num_bytes = read ( serial_port, &read_buf, 3 );

if ( num_bytes == 3 )
{
  printf ( "received\n" );
}
*/

    usleep ( 100000 );
  }

  // clean up and exit
  delwin ( mainwin );
  endwin();
  refresh();
  close ( serial_port );
  return EXIT_SUCCESS;
}


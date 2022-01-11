
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
  WINDOW* mainwin;

  int ch;
  int serial_port;
  int sel_cmd;
  int sel_pad;
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
  sel_pad = 0;
  sel_cmd = 0;

  // show usage
  mvaddstr ( 5, 10, "Press a key, q:quit, s,S:sel pad, c,C:sel command" );
  refresh();

  // loop until user presses q
  while ( ( ch = getch() ) != 'q' )
  {
    // delete the old response lines
    move ( 8, 10 ); deleteln();
    move ( 7, 10 ); deleteln();
    move ( 6, 10 ); deleteln();

    if ( ch == 's' || ch == 'S' ) // change selected pad
    {
      ch == 's' ? sel_pad++ : sel_pad--;
      sel_pad = std::max ( 0, std::min ( max_num_pads - 1, sel_pad ) );
      mvprintw ( 8, 10, "s:sel pad" );
      write(serial_port, get_midi_cmd ( 108, sel_pad ), 3);
    }

    if ( ch == 'c' || ch == 'C' ) // change selected command
    {
      ch == 'c' ? sel_cmd++ : sel_cmd--;
      sel_cmd = std::max ( 0, std::min ( number_cmd - 1, sel_cmd ) );
      mvprintw ( 8, 10, "c:sel command" );
    }

    mvprintw ( 7, 10, "Selected pad: %d",     sel_pad );
    mvprintw ( 6, 10, "Selected command: %s", cmd_names[sel_cmd] );
    refresh();
  }

  // clean up and exit
  delwin ( mainwin );
  endwin();
  refresh();
  close ( serial_port );
  return EXIT_SUCCESS;
}


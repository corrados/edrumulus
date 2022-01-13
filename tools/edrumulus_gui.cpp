
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
int         param_set[]  = {      0,        0,      0,           0,          0,           0,       0,       0,         0,      0,          0,       0 };

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
  struct termios tty, prev_tty;

  // open serial USB port and set correct baud rate
  int serial_port = open ( "/dev/ttyUSB0", O_RDWR | O_NONBLOCK );
  int ret         = tcgetattr ( serial_port, &prev_tty ); // store old tty settings
  tty.c_cflag     = B38400 | CS8 | CLOCAL | CREAD;
  tty.c_iflag     = IGNPAR;
  tty.c_oflag     = 0;
  tty.c_lflag     = 0;
  tty.c_cc[VMIN]  = 1;
  tty.c_cc[VTIME] = 0;
  tcflush ( serial_port, TCIFLUSH );
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
  mvaddstr ( 5, 10, "Press a key; q:quit; s,S:sel pad; c,C:sel command; up,down: change parameter" );
  refresh();

  // loop until user presses q
  while ( ( ch = getch() ) != 'q' )
  {
    if ( ch != -1 )
    {
      // delete the old response lines
      move ( 9, 10 ); deleteln();
      move ( 8, 10 ); deleteln();
      move ( 7, 10 ); deleteln();

      if ( ch == 's' || ch == 'S' ) // change selected pad
      {
        ch == 's' ? sel_pad++ : sel_pad--;
        sel_pad = std::max ( 0, std::min ( max_num_pads - 1, sel_pad ) );
        write ( serial_port, get_midi_cmd ( 108, sel_pad ), 3 );
      }
      else if ( ch == 'c' || ch == 'C' ) // change selected command
      {
        ch == 'c' ? sel_cmd++ : sel_cmd--;
        sel_cmd = std::max ( 0, std::min ( number_cmd - 1, sel_cmd ) );
      }
      else if ( ch == 258 || ch == 259 ) // change parameter value with up/down keys
      {
        ch == 259 ? param_set[sel_cmd]++ : param_set[sel_cmd]--;
        param_set[sel_cmd] = std::max ( 0, std::min ( 31, param_set[sel_cmd] ) );
        write ( serial_port, get_midi_cmd ( cmd_val[sel_cmd], param_set[sel_cmd] ), 3 );
      }

      mvprintw ( 9, 10, "Parameter value:  %d", param_set[sel_cmd] );
      mvprintw ( 8, 10, "Selected pad:     %d", sel_pad );
      mvprintw ( 7, 10, "Selected command: %s", cmd_names[sel_cmd] );
      refresh();
    }

    usleep ( 100000 );
  }

  // clean up and exit
  delwin ( mainwin );
  endwin();
  refresh();
  tcsetattr ( serial_port, TCSANOW, &prev_tty );
  close ( serial_port );
  return EXIT_SUCCESS;
}


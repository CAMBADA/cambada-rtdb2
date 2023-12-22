import functools
import curses
from curses import panel
import json
from rtdb2_panel_controller import RtDBPanelController
from time import gmtime, strftime

class RtDBCurses():
    def __init__(self):
        self.stdscr = curses.initscr()
        self.stdscr.nodelay(1)
        self.stdscr.keypad(1)
        self.TOP_MESSAGE = "RTDB Data Watcher"

        self.ctrl_main = RtDBPanelController()
        self.ctrl_aux = RtDBPanelController()
        self.show_agent_specific = None # None = all, int = show particular agent id
        self.show_details_item = None
        self.show_details_type = 0
        self.sort_item_selected = None  # current_item, selected_item

        self.current_sort_order = ['Agent', 'Key', 'Shared', 'Age', 'Size']

        self.win_details = curses.newwin(0, 0, 0, 0) # h,l,x,y
        self.win_details.addstr(2, 2, "Panel")
        self.panel_details = curses.panel.new_panel(self.win_details)

        curses.start_color() # load colors
        curses.use_default_colors()
        curses.noecho()      # do not echo text
        curses.cbreak()      # do not wait for "enter"
        curses.mousemask(curses.ALL_MOUSE_EVENTS)

        # Hide cursor, if terminal AND curse supports it
        if hasattr(curses, 'curs_set'):
            try:
                curses.curs_set(0)
            except:
                pass

        # Curses colors
        curses.init_pair(1, curses.COLOR_BLACK, curses.COLOR_GREEN)  # header
        curses.init_pair(2, curses.COLOR_BLACK, curses.COLOR_CYAN)  # focused header / line
        curses.init_pair(3, curses.COLOR_WHITE, -1)  # regular
        curses.init_pair(4, curses.COLOR_CYAN, -1)  # tree
        curses.init_pair(5, curses.COLOR_YELLOW, -1)  # tree
        curses.init_pair(6, curses.COLOR_YELLOW, curses.COLOR_CYAN)  # tree

    def listen_keyboard(self, info_list):
        height, width = self.stdscr.getmaxyx()
        height_details, width_details = self.win_details.getmaxyx()
        try:
            self.stdscr.timeout(15)
            key = self.stdscr.getch()
            if key == ord('q') or key == ord('Q'):
                raise KeyboardInterrupt()
            elif key in [curses.KEY_ENTER, ord('\n')]:
                if self.show_details_type == 0:
                    self.show_details_type = 1
                    self.show_details_item = info_list[self.ctrl_main.vpos]
                    self.ctrl_main.hpos = 0
                else:
                    self.show_details_type = 0
                self.ctrl_aux.reset()
                self.sort_item_selected = None
            elif key == ord('s') or key == ord('S'):
                if self.show_details_type == 2:
                    self.sort_item_selected = None if self.ctrl_aux.vpos == self.sort_item_selected \
                        else self.ctrl_aux.vpos
            elif key == ord('t') or key == ord('T'):
                if self.show_details_type == 2 and self.sort_item_selected != None:
                    move_element = self.current_sort_order[self.sort_item_selected]
                    self.current_sort_order[self.sort_item_selected] = self.current_sort_order[self.ctrl_aux.vpos]
                    self.current_sort_order[self.ctrl_aux.vpos] = move_element
                    self.sort_item_selected = None
            elif key == curses.KEY_F3:
                if not self.show_details_type:
                    self.show_details_type = 2
            elif key == curses.KEY_UP:
                if self.show_details_type:
                    self.ctrl_aux.decrement_vertical()
                else:
                    self.ctrl_main.decrement_vertical()
            elif key == curses.KEY_DOWN:
                if self.show_details_type:
                    self.ctrl_aux.increment_vertical()
                else:
                    self.ctrl_main.increment_vertical()
            elif key == curses.KEY_RIGHT:
                if self.show_details_type:
                    self.ctrl_aux.increment_horizontal()
                else:
                    self.ctrl_main.increment_horizontal()
            elif key == curses.KEY_LEFT:
                if self.show_details_type:
                    self.ctrl_aux.decrement_horizontal()
                else:
                    self.ctrl_main.decrement_horizontal()
            elif key == curses.KEY_PPAGE:
                if self.show_details_type:
                    self.ctrl_aux.decrement_vertical(int(height_details / 2))
                else:
                    self.ctrl_main.decrement_vertical(int(height / 2))
            elif key == curses.KEY_NPAGE:
                if self.show_details_type:
                    self.ctrl_aux.increment_vertical(int(height_details / 2))
                else:
                    self.ctrl_main.increment_vertical(int(height / 2))
            elif key == ord('0'):
                self.show_agent_specific = None
            elif key > ord('0') and key <= ord('9'):
                self.show_agent_specific = int(key - ord('0'))
            else:
                return 1
        except curses.error:
            return 0

    def exit_screen(self):
        curses.nocbreak()
        self.stdscr.keypad(0)
        curses.echo()
        curses.endwin()

    def addstr_wrapper(self, window, message, color = None, y = None, x = None):
        height, width = window.getmaxyx()
        if (x is not None and y is not None) and (x >= width or y >= height or x < 0 or y < 0):
            return
        message = message[:width]
        try:
            if y == None or x == None:
                window.addstr(message, color)
            else:
                window.addstr(y, x, message, color)
        except:
            pass

    def display_bottom_bar(self, list_tuples):
        total_size = 0
        height, width = self.stdscr.getmaxyx()
        color = curses.color_pair(2)
        color_white = curses.color_pair(3)
        for instruction, objective in list_tuples:
            self.addstr_wrapper(self.stdscr, instruction, color_white, height - 1, total_size)
            self.addstr_wrapper(self.stdscr, objective, color)
            total_size += len(instruction) + len(objective)
        self.addstr_wrapper(self.stdscr, " " * width, color, height - 1, total_size)

    def display_info(self, info_list):

        # filter on agent id?
        if self.show_agent_specific != None:
            for idx in reversed(range(len(info_list))):
                agent = info_list[idx].agent
                if agent != self.show_agent_specific:
                    del info_list[idx]

        info_list.sort(key=lambda element:
            functools.reduce(lambda r, h: r + [getattr(element, h.lower())], self.current_sort_order, [])
        )
        self.listen_keyboard(info_list)

        self.stdscr.erase()
        self.win_details.erase()

        height, width = self.stdscr.getmaxyx()

        color = curses.color_pair(2)
        color_white = curses.color_pair(3)
        color_title = curses.color_pair(4)
        # This displays the message on the top
        self.addstr_wrapper(self.stdscr, " " * width, curses.color_pair(1), 0, 0)
        message = self.TOP_MESSAGE + " " + strftime("%Y-%m-%d %H:%M:%S", gmtime())
        self.addstr_wrapper(self.stdscr, message, curses.color_pair(1), 0,
                            int(width / 2 - len(message) / 2))

        # This displays the current menu
        self.ctrl_main.max_vpos = len(info_list)

        format_header = "%5s %-30s %-6s %6s %5s %-s"
        format_data   = "%5s %-30s %-6s %6s %5d %-s"
        # Checking the maximum value that will be possible to scroll to the right with the new values
        data_list = []
        self.ctrl_main.max_hpos = 0
        for item in info_list:
            try:
                str_data = format_data % \
                    (item.agent, item.key, ["local", "shared"][item.shared], item.agef(), item.size, item.value)
            except:
                str_data = str((item.agent, item.key, ["local", "shared"][item.shared], item.agef(), item.size, item.value))
            data_list.append(str_data)
            if len(str_data) - width + 1 > self.ctrl_main.max_hpos:
                self.ctrl_main.max_hpos = len(str_data) - width + 1

        # Fill the mid panels
        self.addstr_wrapper(self.stdscr, " " * width, color, 1, 0)
        heading_str = format_header % \
                ("Agent", "Key", "Type", "Age  ", "Size", "Value")
        self.addstr_wrapper(self.stdscr, heading_str[self.ctrl_main.hpos:], color, 1, 0)
        for idx, value in enumerate(info_list):
            if idx == self.ctrl_main.vpos:
                self.addstr_wrapper(self.stdscr, " " * width, curses.color_pair(1), idx + 2, 0)
            self.addstr_wrapper(self.stdscr, data_list[idx][self.ctrl_main.hpos:],
                                curses.color_pair(1) if idx == self.ctrl_main.vpos else color_white,
                                idx + 2, 0)

        # Behaviour for the details window
        if self.show_details_type:
            self.panel_details.top()

        if self.show_details_type == 1:
            self.display_bottom_bar([
                ("Enter", "Close details"),
                ("Q", "Quit")
            ])

            self.win_details.resize(height - 6, width - 14)
            self.panel_details.move(3, 7)

            height_details, width_details = self.win_details.getmaxyx()
            data = json.dumps(self.show_details_item.value, indent = 4, sort_keys=True).split('\n')
            self.ctrl_aux.max_vpos = len(data) - height_details + 3
            self.ctrl_aux.max_hpos = 0
            for value in data:
                if len(value) - width_details + 3 > self.ctrl_aux.max_hpos:
                    self.ctrl_aux.max_hpos = len(value) - width_details + 3
            for idx, line in enumerate(data):
                self.addstr_wrapper(self.win_details, line[self.ctrl_aux.hpos:], color_white,
                                    idx + 1 - self.ctrl_aux.vpos, 1)
        elif self.show_details_type == 2:
            self.display_bottom_bar([
                ("S", "(Un)select item to move"),
                ("T", "Trade items"),
                ("Enter", "Finish sort order"),
                ("Q", "Quit")
            ])

            self.win_details.resize(len(self.current_sort_order) + 4, 40)
            self.addstr_wrapper(self.win_details, "Define the order of the elements", color_title, 1, 4)
            self.addstr_wrapper(self.win_details, "S to select/unselect, T to trade", color_white, 2, 4)
            self.panel_details.move(3, 7)

            height_details, width_details = self.win_details.getmaxyx()
            self.ctrl_aux.max_vpos = len(self.current_sort_order)
            self.ctrl_aux.max_hpos = 0

            self.addstr_wrapper(self.win_details, " " * width_details, color, self.ctrl_aux.vpos + 4, 1)
            for idx, value in enumerate(self.current_sort_order):
                if idx == self.ctrl_aux.vpos:
                    self.addstr_wrapper(self.win_details, str(idx + 1) + ". " + value,
                                        curses.color_pair(6) | curses.A_BOLD if self.sort_item_selected != None and
                                                                self.sort_item_selected == idx else color,
                                        idx + 4, 3)
                elif self.sort_item_selected != None and self.sort_item_selected == idx:
                    self.addstr_wrapper(self.win_details, str(idx + 1) + ". " + value,
                                        curses.color_pair(5) | curses.A_BOLD, idx + 4, 3)
                else:
                    self.addstr_wrapper(self.win_details, str(idx + 1) + ". " + value, color_white, idx + 4, 3)


        else:
            self.display_bottom_bar([
                ("F3", "Sort"),
                ("Enter", "Show details"),
                ("Q", "Quit")
            ])

            self.panel_details.bottom()
            self.panel_details.hide()

        if self.show_details_type:
            self.win_details.box()
            curses.panel.update_panels()

        self.stdscr.refresh()

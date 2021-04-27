import npyscreen
import curses
import argparse
import sys
import copy
from rtdb2 import RtDB2Store, RTDB2_DEFAULT_PATH
import rtdb2tools


class TreeDataConfig(npyscreen.TreeData):

    def __init__(self, *args, **keywords):
        super(TreeDataConfig, self).__init__(*args, **keywords)

    def get_content_for_display(self):
        if self.content["value"] is not None:
            return "%s: %s" % (self.content["name"], str(self.content["value"]))
        else:
            return str( self.content["name"] )


class MLTreeConfig(npyscreen.MLTree):

    _contained_widgets = npyscreen.TreeLine

    def __init__(self, *args, **keywords):
        super(MLTreeConfig, self).__init__(*args, **keywords)

        self.handlers.update ( {
                ord('h'):           self.display_help,
                ord('q'):           self.h_exit,
                curses.ascii.NL:    self.display_menu, #NewLine
                curses.ascii.SP:    self.display_menu, #Space
                curses.ascii.ESC:   self.h_exit,
                curses.ascii.CR:    self.display_menu, #CarriageReturn (=Enter)
            } )

    def display_help(self, *args, **kwargs):

        message = ["This tool allows you to edit an RtDB item in the database.",
                   "",
                   "Keyboard shortcuts:",
                   ">:     Expand tree item",
                   "<:     Collapse tree item",
                   "TAB:   Jump to next item",
                   "ENTER: Edit selected value"
                   ]

        F = npyscreen.fmPopup.PopupWide(name="Help", color="STANDOUT")
        F.preserve_selected_widget = True
        F.add(npyscreen.MultiLine, values=message, editable=False)
        F.edit()

    def display_menu(self, *args, **kwargs):
        
        # Obtain the currently selected entry from the Tree
        treeLineEntry = self._my_widgets[self.cursor_line-self.start_display_at]
        treeDataConfig = treeLineEntry._tree_real_value
        treeDataName = treeDataConfig.content["name"]
        treeDataValue = treeDataConfig.content["value"]

        # Group names have value None -- these do not have a value to edit
        if treeDataValue is not None:

            F = npyscreen.utilNotify.ConfirmCancelPopup(name="Modify Value", color="STANDOUT")
            F.preserve_selected_widget = True
            wid1 = F.add(npyscreen.TitleText, name="Name", value=treeDataName, use_two_lines=False, editable=False )
            wid2 = F.add(npyscreen.TitleText, name="Current value", value=str(treeDataValue), use_two_lines=False, editable=False )
            if type(treeDataValue) == float:
                wid3 = F.add(npyscreen.TitleText, name="New value", value=str(treeDataValue), use_two_lines=False, editable=True )
            elif type(treeDataValue) == str:
                wid3 = F.add(npyscreen.TitleText, name="New value", value=str(treeDataValue), use_two_lines=False, editable=True )
            elif type(treeDataValue) == bool:
                wid3 = F.add(npyscreen.TitleSelectOne, name="New value", value = [bool(treeDataValue),], values = ["False","True"], scroll_exit=True)
            elif type(treeDataValue) == int:
                wid3 = F.add(npyscreen.TitleText, name="New value", value=str(treeDataValue), use_two_lines=False, editable=True )
            
            F.edit()

            # Only do something when OK was selected
            if F.value:
                
                if type(treeDataValue) == float:
                    newvalue = wid3.value
                elif type(treeDataValue) == str:
                    newvalue = wid3.value
                elif type(treeDataValue) == bool:
                    newvalue = wid3.value[0]
                elif type(treeDataValue) == int:
                    newvalue = wid3.value
                #self._my_widgets[self.cursor_line-self.start_display_at]._tree_real_value.content["value"] = newvalue

                # Push to RtDB and update table
                self.parent.dataHandler.updateValue(treeDataConfig.content["name"], newvalue, treeDataConfig.content["parentBreadcrumbList"])
                self.values = self.parent.dataHandler.getData()
                self.update()

    def convertToTree(self, data):
        treedata = TreeDataConfig(content='Root', selectable=False, ignore_root=True)
        self.buildTree(data, treedata, [])

        return treedata

    # The parentBreadCrumbList keeps track of all parents.
    # This is necessary when a value is edited such that the correct value is updated and the whole Dict can be written back to RtDB
    def buildTree(self, dataDict, root, parentBreadcrumbList):

        for key in sorted(dataDict.keys()):

            if type( dataDict[key] ) == dict:
                # Found a new dict, add Title child and add all children recursively
                newentry = root.new_child(content={'name': key, 'value': None}, selectable=False)
                newParentBreadcrumbList = copy.deepcopy(parentBreadcrumbList)
                newParentBreadcrumbList.append(key)
                self.buildTree( dataDict[key], newentry, newParentBreadcrumbList )
            else:
                # Found a value, add Child
                newentry = root.new_child(content={'name': key, 'value': dataDict[key], 'parentBreadcrumbList': copy.deepcopy(parentBreadcrumbList)}, selectable=True)


class TestApp(npyscreen.NPSApp):

    def setDataHandler(self, dataHandler):
        self.dataHandler = dataHandler

    def main(self):
        F = npyscreen.Form(name = "Robot %s: %s -- Press 'h' for help" % (str(self.dataHandler.agent), self.dataHandler.key),)
        wgtree = F.add(MLTreeConfig)

        # Get the latest rtdb data from the DataHandler
        wgtree.values = self.dataHandler.getData()

        F.dataHandler = self.dataHandler

        F.edit()

class RtDBDataHandler():

    def __init__(self, path, agent, key):

        self.agent = agent
        self.key = key
        
        # Create instance of RtDB2Store and read databases from disk
        self.rtdb2Store = RtDB2Store(path, False) # don't start in read-only

    def getData(self):
        # Get current value
        rtdbEntry = self.rtdb2Store.get(self.agent, self.key, timeout=None)
        if rtdbEntry is not None:
            self.rtdbData = rtdbEntry.value
        else:
            sys.exit("ERROR: Data is None. agent='%s', key='%s'" % (self.agent, self.key))

        return self.rtdbData

    # Update a value given the key, newvalue, and the parentBreadcrumbList to find the value to update in the Dict
    def updateValue(self, key, newvalue, parentBreadcrumbList):
        # Root
        currentEntry = self.rtdbData

        # Go through all parents to update the value
        for parent in parentBreadcrumbList:
            currentEntry = currentEntry[parent]

        # Reached child
        if type(currentEntry[key]) == float:
            currentEntry[key] = float(newvalue)
        elif type(currentEntry[key]) == str:
            currentEntry[key] = str(newvalue)
        elif type(currentEntry[key]) == bool:
            currentEntry[key] = bool(newvalue)
        elif type(currentEntry[key]) == int:
            currentEntry[key] = int(newvalue)

        # Write to rtdb
        self.rtdb2Store.put(self.agent, self.key, self.rtdbData)

if __name__ == "__main__":

    # Argument parsing.
    descriptionTxt = 'This tool allows you to edit an RtDB item in the database given an RtDB key.\n'
    exampleTxt = 'Example: rtdb2_edit.py CONFIG_PATHPLANNING\n'
    parser     = argparse.ArgumentParser(description=descriptionTxt, epilog=exampleTxt,  formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('-a', '--agent', help='agent ID to use', type=int, default=rtdb2tools.guessAgentId())
    parser.add_argument('-p', '--path', help='database path to use', type=str, default=RTDB2_DEFAULT_PATH)
    parser.add_argument('key', help='RtDB key to edit')
    args       = parser.parse_args()

    dataHandler = RtDBDataHandler(args.path, args.agent, args.key)

    App = TestApp()
    App.setDataHandler(dataHandler)
    App.run()


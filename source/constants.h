 char * stringFunction[] ={"Not used",
                            "Add a user",
                            "Delete a user", 
                            "Alter a user", 
                            "List a user", 
                            "Run a RACF command", 
                            "Add a group", 
                            "Delete a group", 
                            "Alter a group", 
                            "List a group", 
                            "Connect user to group", 
                            "Remove user from group", 
                            "Add a resource", 
                            "Delete a resource", 
                            "Alter a resource", 
                            "List a resource", 
                            "Add a dataset", 
                            "Delete a dataset", 
                            "Alter a dataset", 
                            "List a dataset", 
                            "Permit a user or group", 
                            "Alter SETROPTS info", 
                            "SETROPTS R_admin ext", 
                            "SETROPTS SMF Unload ext", 
                            "Extract Pw Envelope", 
                            "Extract User", 
                            "Extract Next user", 
                            "Extract Group", 
                            "Extract Next Group", 
                            "Extract Connect", 
                            "Extract Phrase Envelope", 
                            "Extract General Resource Profile", 
                            "Extract Next General Resource Profile", 
                            "Extract RRSF info", 
                            "Extract dataset", 
                            "Extract next dataset" };
 char * cFunction(char v)
 {
  int i = v;
  int max = sizeof(stringFunction)/sizeof(stringFunction[0]);
  char * p = stringFunction[i];
  return p;
 }

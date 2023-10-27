# Litera
Litera is a work in progress Evernote client written in C with GTK4. There are planty of them, but most of the Evernote clients I've found based on web technologies. This one is a native application without web view. At least I want to build such. Native text editing will consume less resources and will be more efficient then a web browser. Also I've chosen C and GTK as I'm using mosty GTK applications and thought that it would look fine along with them and create a consistent look for the system. I'm not planning to add a fancy UI to look like creative tool note taking. It is rather simplistic application build with default controls. In the future it possible to adopt libadwaita to look better on GNOME, but the application is too far away and is not usable currently to think about styling.

I will be happy in case 0.1 version will be complete and it will be possible to use the app for basic text editing. However I'm writing the app in the way that will allow to add more backends for notes (for example the file system). Maybe, in the future, other backends will be added as well.

## Features for 0.1
- [ ] **Proper user authorization.** It should be possible to login to any account without developer token.
- [x] **Read the list of notebooks and notes in it.** It is now possible to get the list of notebooks and notes and visualize raw content of the note (Evernote uses XHTML to store content of the note).
- [ ] **Present content of the note.** XHTML should be parsed and presented in the way it was intended to look by the author.
- [ ] **Edit the text within the note save changes on the server**. User should be able to edit text in the not and save changes.
- [ ] **Logout**. User should be able to logout from the application.

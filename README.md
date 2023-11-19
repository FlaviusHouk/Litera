# Litera
Litera is a work in progress Evernote client written in C with GTK4. There are plenty of them, but most of the Evernote clients I've found based on web technologies. This one is a native application without web view. Native text editing will consume less resources and will be more efficient then a web browser. Also I've chosen C and GTK as I'm using mosty GTK applications and thought that it would look fine along with them and create a consistent look for the system. I'm not planning to add a fancy UI to look like creative tool for note taking. It is rather simplistic application build with default controls. In the future it possible to adopt libadwaita to look better on GNOME, but the application is too far away and is not usable currently to think about styling.

I will be happy in case 0.1 version will be complete and it will be possible to use the app for basic text editing. However I'm writing the app in the way that will allow to add more backends for notes (for example the file system). Maybe, in the future, other backends will be added as well.

While the sandbox for the Evernote (sandbox.evernote.com) does not work, I'm condidering adding a file system backend for this. It might be useful to develop other features, not related to Evernote. In future it would be nice to do a backup or data migration between these sources.

## Features for 0.1
- [ ] **Proper user authorization.** It should be possible to login to any account without developer token.
- [x] **Read the list of notebooks and notes in it.** It is now possible to get the list of notebooks and notes and visualize text in the note. 
- [x] **Present content of the note.** Images, list and other parts of the note will cause an application to crash. (Support for this items is planned).
- [ ] **Edit the text within the note save changes on the server**. User should be able to edit text in the not and save changes.
- [ ] **Logout**. User should be able to logout from the application.

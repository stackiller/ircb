/*
  Moderate Channel,
  functions

  &Brules, &Bregister, &Bidentify,
  &Babout, &say,
*/

// Messages
char wiki_rules[] = "The rules are the dont are idiot ^__^?";
char wiki_register[] = "Register Command is: /msg nickserv register <password> <email>";
char wiki_identify[] = "Identify command: /msg nickserv identify <your_password>";
char wiki_about[] = "Hi how you are about know me ? ^__^";
char wiki_say[] = "Hi you are wonderful, ^__^";
char wiki_help[] = "Hi need awesome help ?";

// Brules - rules of channel
void Brules(char dst[]) {
	priv_msg(wiki_rules, dst);
}
// Bregister - register wiki
void Bregister(char dst[]) {
	priv_msg(wiki_register, dst);
}
// Bidentify - identify wiki
void Bidentify(char dst[]) {
	priv_msg(wiki_identify, dst);
}
// Brules - rules of channel
void Babout(char dst[]) {
	priv_msg(wiki_about, dst);
}
// Bsay - rules of channel
void Bsay(char dst[]) {
	priv_msg(wiki_say, dst);
}
// Bsay - rules of channel
void Bhelp(char dst[]) {
	priv_msg(wiki_help, dst);
}

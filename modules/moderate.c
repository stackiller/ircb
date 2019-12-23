/*
  Moderate Channel,
  functions

  &Brules, &Bregister, &Bidentify,
  &Babout, &say,
*/

// Messages
char wiki_rules[] = "rules";
char wiki_register[] = "register";
char wiki_identify[] = "identify";
char wiki_about[] = "about";
char wiki_say[] = "say";
char wiki_help[] = "help";

// Brules - rules of channel
void
Brules(char dst[])
{
  priv_msg(wiki_rules, dst);
}

// Bregister - register wiki
void
Bregister(char dst[])
{
  priv_msg(wiki_register, dst);
}

// Bidentify - identify wiki
void
Bidentify(char dst[])
{
  priv_msg(wiki_identify, dst);
}

// Brules - rules of channel
void
Babout(char dst[])
{
  priv_msg(wiki_about, dst);
}

// Bsay - rules of channel
void
Bsay(char dst[])
{
  priv_msg(wiki_say, dst);
}

// Bsay - rules of channel
void
Bhelp(char dst[])
{
  priv_msg(wiki_help, dst);
}

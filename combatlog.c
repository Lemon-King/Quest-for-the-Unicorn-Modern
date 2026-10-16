/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

/* The message log's lines for combat and its events. With the Improved Combat Logs preference each line says who did
   what to whom; Off, each keeps the text and colour the game first gave it. */

#include "header.h"

#define NAME_SLOTS        4
#define NAME_SIZE        48
#define ITEM_NAME_SIZE  128
#define TRAP_COUNT      (TRAP_CONFUSION - TRAP_SMALL_EXPLOSIVE + 1)
#define PROPER_COUNT    ((Int32)(sizeof(PROPER_NAMES) / sizeof(PROPER_NAMES[0])))

/* Creatures with names of their own, which take no article */
static char* PROPER_NAMES[] = { "Juiblex", "Orcus", "Demogorgon", "Count Tralin", "Caldnar", "Elanther", FINAL_BOSS };

static char* SKILL_NAMES[SK_THROWN + 1] = {
  [SK_BOW]      = "Bow",      [SK_1HSWORD]  = "1H Sword", [SK_SPEAR]   = "Spear",    [SK_MACE]    = "Mace",
  [SK_SCOUT]    = "Scouting", [SK_LANG]     = "Language", [SK_LORE]    = "Lore",     [SK_MAGIC]   = "Magic",
  [SK_HEAL]     = "Healing",  [SK_MED]      = "Medicine", [SK_RIDE]    = "Riding",   [SK_BOAT]    = "Boating",
  [SK_ORACLE]   = "Oracle",   [SK_CHARM]    = "Charm",    [SK_SURVIVE] = "Survival", [SK_COMBAT]  = "Combat",
  [SK_DAGGER]   = "Dagger",   [SK_2HSWORD]  = "2H Sword", [SK_AXE]     = "Axe",      [SK_FLAIL]   = "Flail",
  [SK_CROSSBOW] = "Crossbow", [SK_TRIDENT]  = "Trident",  [SK_POLEAXE] = "Poleaxe",  [SK_THROWN]  = "Thrown"
  };

/* The level-up's names for the skills up to Combat, from before the weapon skills were split */
static char* OLD_SKILL_NAMES[SK_COMBAT + 1] = {
  [SK_BOW]      = "Missile",  [SK_1HSWORD]  = "Edge",     [SK_SPEAR]   = "Pole",     [SK_MACE]    = "Blunt",
  [SK_SCOUT]    = "Scouting", [SK_LANG]     = "Language", [SK_LORE]    = "Lore",     [SK_MAGIC]   = "Magic",
  [SK_HEAL]     = "Healing",  [SK_MED]      = "Medicine", [SK_RIDE]    = "Riding",   [SK_BOAT]    = "Boating",
  [SK_ORACLE]   = "Oracle",   [SK_CHARM]    = "Charm",    [SK_SURVIVE] = "Survival", [SK_COMBAT]  = "Combat"
  };

static char* ATTRIBUTE_NAMES[ATTR_CHARISMA + 1] = {
  [ATTR_STRENGTH] = "Strength", [ATTR_DEXTERITY] = "Dexterity", [ATTR_CONSTITUTION] = "Constitution",
  [ATTR_INTELLIGENCE] = "Intelligence", [ATTR_WISDOM] = "Wisdom", [ATTR_CHARISMA] = "Charisma"
  };

/* The player's traps in PLAYER_TRAP order: their kinds, and what each shouted when its blast caught the player */
static char* TRAP_KINDS[TRAP_COUNT] = {
  "explosive", "explosive", "explosive", "fire", "frost", "electric", "sleep", "fear", "blindness", "poison",
  "confusion"
  };
static char* TRAP_SHOUTS[TRAP_COUNT] = {
  "OUCH!!!", "OUCH!!!", "OUCH!!!", "BURNING!!!", "ROOTED!!!", "ZAP!!!", "SLEPT!!!", "SCARED!!!", "BLIND!!!",
  "POISONED!!!", "CONFUSED!!!"
  };

static char* OLD_ALTAR_BOONS[ALTAR_BOONS] = {
  [ALTAR_STRENGTH]   = "STR increased",  [ALTAR_DEXTERITY] = "DEX increased", [ALTAR_CONSTITUTION] = "CON increased",
  [ALTAR_HIT_POINTS] = "HP increased",   [ALTAR_MANA]      = "Mana increased",
  [ALTAR_SKILL]      = "Skill increased", [ALTAR_CHARISMA] = "CHR increased"
  };

static Int32 improved(void) {
  return (improved_combat_logs == 'Y');
  }

/* ***** Names ***** */

/* Names are made in a ring of buffers, so one line can name several creatures */
static char* name_slot(void) {
  static char  slots[NAME_SLOTS][NAME_SIZE];
  static Int32 next = 0;

  next = (next + 1) % NAME_SLOTS;
  return slots[next];
  }

static Int32 is_proper_name(char* name) {
  Int32 i;

  for (i=0; i<PROPER_COUNT; i++)
    if (strcmp(name,PROPER_NAMES[i]) == 0) return 1;

  return 0;
  }

static Int32 starts_with(char* name,char* prefix) {
  return (strncmp(name,prefix,strlen(prefix)) == 0);
  }

/* "an" for a vowel's sound: an Orc, an 8 head Hydra, an 11 head Hydra, but a Unicorn */
static Int32 takes_an(char* name) {
  if (name[0] == 0 || starts_with(name,"Uni")) return 0;
  if (strchr("AEIOUaeiou8",name[0]) != NULL) return 1;
  return starts_with(name,"11");
  }

/* "the Orc", or "The Orc" to start a line; a named creature as it is */
char* the_name(char* name,NAME_POSITION position) {
  char* slot;

  slot = name_slot();
  if (is_proper_name(name)) snprintf(slot,NAME_SIZE,"%s",name);
    else snprintf(slot,NAME_SIZE,"%s %s",(position == LINE_START) ? "The" : "the",name);

  return slot;
  }

/* "a Ghoul", "an Orc"; a named creature as it is */
char* a_name(char* name) {
  char* slot;

  slot = name_slot();
  if (is_proper_name(name)) snprintf(slot,NAME_SIZE,"%s",name);
    else snprintf(slot,NAME_SIZE,"%s %s",takes_an(name) ? "an" : "a",name);

  return slot;
  }

static Int32 is_skill(Int32 skill) {
  return (skill >= SK_BOW && skill <= SK_THROWN);
  }

static Int32 attribute_value(ATTRIBUTE stat) {
  switch (stat) {
    case ATTR_STRENGTH:     return get_str();
    case ATTR_DEXTERITY:    return get_dex();
    case ATTR_CONSTITUTION: return get_con();
    case ATTR_INTELLIGENCE: return get_int();
    case ATTR_WISDOM:       return get_wis();
    case ATTR_CHARISMA:     return get_chr();
    }

  return 0;
  }

/* "7", or "7 fire damage" for elem " fire ", the old lines' word for an element */
static char* damage_words(Int32 damage,char* elem) {
  static char words[NAME_SIZE];

  if (elem[0] == ' ') snprintf(words,sizeof(words),"%d %sdamage",damage,elem + 1);
    else snprintf(words,sizeof(words),"%d",damage);

  return words;
  }

/* ***** The attack source: the spell, item or weapon the player's attack uses ***** */

void set_attack_source(char* name) {
  snprintf(attack_source,sizeof(attack_source),"%s",name);
  }

/* The item an attack uses, by its plain name after prefix: "thrown Dagger" */
void set_attack_item(char* prefix,OBJECT_REC* item) {
  char name[ITEM_NAME_SIZE];

  snprintf(attack_source,sizeof(attack_source),"%s%s",prefix,base_name(name,item));
  }

void clear_attack_source(void) {
  attack_source[0] = 0;
  }

/* ***** The player's attacks ***** */

static void improved_hit(NPC_REC* target,Int32 damage,Int32 crit) {
  char* who;

  who = the_name(target->name,MID_LINE);
  if (attack_source[0] == 0) {
    if (crit) msgf(CLR_YELLOW,"Critical! You hit %s for %d",who,damage);
      else msgf(CLR_LIGHTGREEN,"You hit %s for %d",who,damage);
    return;
    }

  if (crit) msgf(CLR_YELLOW,"Critical! Your %s hits %s for %d",attack_source,who,damage);
    else msgf(CLR_LIGHTGREEN,"Your %s hits %s for %d",attack_source,who,damage);
  }

static void improved_miss(NPC_REC* target,MISS_KIND kind) {
  if (kind == RESISTED) {
    if (attack_source[0] == 0) msgf(CLR_GRAY,"%s is unharmed by your attack",the_name(target->name,LINE_START));
      else msgf(CLR_GRAY,"%s resists your %s",the_name(target->name,LINE_START),attack_source);
    return;
    }

  if (attack_source[0] == 0) msgf(CLR_GRAY,"You miss %s",the_name(target->name,MID_LINE));
    else msgf(CLR_GRAY,"Your %s misses %s",attack_source,the_name(target->name,MID_LINE));
  }

/* A blow, shot or missile spell that lands, through aoe() or touch() */
void log_hit(NPC_REC* target,Int32 damage,Int32 crit) {
  if (improved()) improved_hit(target,damage,crit);
    else if (crit) msgf(CLR_LIGHTGREEN,"Crit hit for %d",damage);
    else msgf(CLR_LIGHTGREEN,"Hit for %d",damage);
  }

void log_wand_hit(NPC_REC* target,Int32 damage,Int32 crit) {
  if (improved()) improved_hit(target,damage,crit);
    else if (crit) msgf(CLR_RED,"CRIT for %d",damage);
    else msgf(CLR_RED,"HIT for %d",damage);
  }

/* The old shout as an attack lands, before its damage is known; the improved log names the damage after */
void log_hit_shout(void) {
  if (!improved()) prompt("HIT !!!");
  }

/* An area, drain or reflected spell's damage, which the old log never named */
void log_spell_hit(NPC_REC* target,Int32 damage,Int32 crit) {
  if (improved()) improved_hit(target,damage,crit);
  }

/* An attack that misses the creature, or that it resists */
void log_miss(NPC_REC* target,MISS_KIND kind) {
  if (improved()) improved_miss(target,kind);
    else prompt("Fizzle. . .");
  }

/* A creature resists a spell of blindness, sleep or fear */
void log_resist(NPC_REC* target) {
  if (improved()) improved_miss(target,RESISTED);
    else prompt("Fizzle. . . ");
  }

void log_melee_miss(NPC_REC* target) {
  if (improved()) improved_miss(target,MISSED);
    else msg("Miss . . .",CLR_LIGHTBLUE);
  }

/* A shot, throw or wand misses */
void log_shot_miss(NPC_REC* target) {
  if (improved()) improved_miss(target,MISSED);
    else prompt("Missed. . .");
  }

/* The Monk's paralysing blow, and the instant kill it can bring */
void log_paralyzing_blow(NPC_REC* target) {
  if (improved()) msgf(CLR_YELLOW,"Your blow paralyzes %s",the_name(target->name,MID_LINE));
    else msg("Paralyzed!",CLR_YELLOW);
  }

void log_killing_blow(NPC_REC* target) {
  if (improved()) msgf(CLR_WHITE,"Your blow kills %s outright",the_name(target->name,MID_LINE));
    else msg("Killed!",CLR_WHITE);
  }

/* ***** What the player's spells and potions do to creatures ***** */

static void creature_effect(NPC_REC* target,char* effect,char* old_text) {
  if (improved()) msgf(CLR_LIGHTGREEN,"%s %s",the_name(target->name,LINE_START),effect);
    else prompt(old_text);
  }

void log_creature_healed(NPC_REC* target,Int32 amount) {
  if (improved()) msgf(CLR_LIGHTGREEN,"%s is healed for %d",the_name(target->name,LINE_START),amount);
    else prompt("Healed !!!");
  }

/* Sleep and fear from potions and area effects; the Sleep and Fear spells shouted louder */
void log_creature_asleep(NPC_REC* target) {
  creature_effect(target,"falls asleep","Slept !!!");
  }

void log_sleep_spell(NPC_REC* target) {
  creature_effect(target,"falls asleep","SLEPT !!!");
  }

void log_creature_afraid(NPC_REC* target) {
  creature_effect(target,"flees in terror","Scared !!!");
  }

void log_fear_spell(NPC_REC* target) {
  creature_effect(target,"flees in terror","SCARED!!!");
  }

void log_creature_blinded(NPC_REC* target) {
  creature_effect(target,"is blinded","Blinded !!!");
  }

void log_creature_confused(NPC_REC* target) {
  creature_effect(target,"is confused","Confused !!!");
  }

void log_creature_poisoned(NPC_REC* target) {
  creature_effect(target,"is poisoned","Poisoned !!!");
  }

void log_charmed(NPC_REC* target) {
  if (improved()) msgf(CLR_LIGHTGREEN,"You charm %s",the_name(target->name,MID_LINE));
    else prompt("Charmed");
  }

/* A charm on a creature already friendly, or on no creature */
void log_charm_failed(NPC_REC* target) {
  if (!improved()) prompt("Fizzle");
    else if (target == NULL) msg("There's no one there to charm",CLR_GRAY);
    else msgf(CLR_GRAY,"%s is already friendly",the_name(target->name,LINE_START));
  }

void log_pacified(NPC_REC* target) {
  if (improved()) msgf(CLR_WHITE,"%s is pacified",the_name(target->name,LINE_START));
    else msg("Pacified",CLR_WHITE);
  }

void log_creature_fear_over(NPC_REC* creature) {
  if (improved()) msgf(CLR_GRAY,"%s is no longer afraid",the_name(creature->name,LINE_START));
    else msg("Mob no longer scared",CLR_GRAY);
  }

/* A poisoned creature's turn: 7.0's leftover debug lines, which the improved log leaves out */
void log_creature_poison_tick(NPC_REC* creature) {
  if (improved()) return;
  msg("here",CLR_WHITE);
  msgf(CLR_WHITE,"poison=%d",creature->poison);
  }

void log_creature_poison_done(void) {
  if (!improved()) msg("done",CLR_WHITE);
  }

/* ***** Kills and experience ***** */

void log_slain(NPC_REC* slain,double exp) {
  if (improved()) msgf(CLR_WHITE,"%s dies! (+%.0f exp)",the_name(slain->name,LINE_START),exp);
    else msgf(CLR_WHITE,"Exp=%.0f",exp);
  }

/* A kill by spell without the Spell Kills change, which gives the creature's experience unshared */
void log_spell_slain(NPC_REC* slain) {
  if (improved()) msgf(CLR_WHITE,"%s dies! (+%.0f exp)",the_name(slain->name,LINE_START),slain->exp);
    else msgf(CLR_LIGHTGREEN,"Exp=%.0f",slain->exp);
  }

void log_ally_level(NPC_REC* member) {
  if (improved()) msgf(CLR_WHITE,"Your %s has reached level %d!",member->name,member->level);
    else msgf(CLR_WHITE,"%s leveled",member->name);
  }

/* ***** Creatures attack the player ***** */

void log_dodge(NPC_REC* attacker,ATTACK_FORM form) {
  if (!improved()) msg("Dodged",CLR_GRAY);
    else if (form == BY_BOLT) msgf(CLR_GRAY,"You dodge %s's bolt",the_name(attacker->name,MID_LINE));
    else msgf(CLR_GRAY,"You dodge %s",the_name(attacker->name,MID_LINE));
  }

/* elem is "", or an element's word with a space each side */
void log_creature_hit(NPC_REC* attacker,Int32 damage,char* elem) {
  if (improved()) msgf(CLR_LIGHTRED,"%s hits you for %s",the_name(attacker->name,LINE_START),damage_words(damage,elem));
    else msgf(CLR_LIGHTRED,"OUCH! for %d%s",damage,elem);
  }

/* A blow from a creature whose wounds hurt over time */
void log_creature_dot(NPC_REC* attacker,Int32 damage) {
  if (improved()) msgf(CLR_LIGHTRED,"%s wounds you for %d over time",the_name(attacker->name,LINE_START),damage);
    else msg("DOT taken",CLR_LIGHTRED);
  }

/* The old log said Missed after a dodge too */
void log_creature_miss(NPC_REC* attacker,Int32 dodged) {
  if (!improved()) msg("Missed . . .",CLR_BLUE);
    else if (!dodged) msgf(CLR_GRAY,"%s misses you",the_name(attacker->name,LINE_START));
  }

void log_creature_bolt(NPC_REC* attacker,Int32 damage,char* elem) {
  if (improved())
    msgf(CLR_LIGHTRED,"%s's bolt hits you for %s",the_name(attacker->name,LINE_START),damage_words(damage,elem));
  else msgf(CLR_LIGHTRED,"ZAP for %d%s",damage,elem);
  }

void log_bolt_miss(NPC_REC* attacker,Int32 dodged) {
  if (!improved()) prompt("Fizzle. . .");
    else if (!dodged) msgf(CLR_GRAY,"%s's bolt misses you",the_name(attacker->name,LINE_START));
  }

/* A ring of reflection turns a bolt; the old log named only the reflected bolt's hit */
void log_bolt_reflected(OBJECT_REC* ring) {
  char name[ITEM_NAME_SIZE];

  if (improved()) msgf(CLR_LIGHTCYAN,"Your %s turns the bolt back",base_name(name,ring));
  }

/* A ring of reflection melts; again is a second melting of the same ring, which the old log shouted too */
void log_ring_melted(OBJECT_REC* ring,Int32 again) {
  char name[ITEM_NAME_SIZE];

  if (!improved()) prompt((in_arena == 'Y') ? "Ring Melted!!" : "Ring Melted!!!");
    else if (!again) msgf(CLR_LIGHTRED,"Your %s melts!",base_name(name,ring));
  }

void log_rooted(NPC_REC* attacker) {
  if (improved()) msgf(CLR_LIGHTBLUE,"%s's attack roots you in place",the_name(attacker->name,LINE_START));
    else msg("Rooted!!!",CLR_LIGHTBLUE);
  }

void log_slept(NPC_REC* attacker) {
  if (improved()) msgf(CLR_LIGHTBLUE,"%s's attack puts you to sleep",the_name(attacker->name,LINE_START));
    else msg("Slept!!!",CLR_LIGHTBLUE);
  }

void log_poisoned(NPC_REC* attacker) {
  if (improved()) msgf(CLR_LIGHTPURPLE,"%s poisons you",the_name(attacker->name,LINE_START));
    else msg("Poisoned!!!",CLR_LIGHTPURPLE);
  }

void log_scared(NPC_REC* attacker) {
  if (improved()) msgf(CLR_LIGHTBLUE,"%s fills you with fear",the_name(attacker->name,LINE_START));
    else msg("Scared!!!",CLR_LIGHTBLUE);
  }

void log_confused(NPC_REC* attacker) {
  if (improved()) msgf(CLR_LIGHTBLUE,"%s confuses you",the_name(attacker->name,LINE_START));
    else msg("Confused!!!",CLR_LIGHTBLUE);
  }

/* After the level is lost */
void log_level_drained(NPC_REC* attacker) {
  if (improved())
    msgf(CLR_LIGHTRED,"%s drains a level! You're now level %d",the_name(attacker->name,LINE_START),player.level);
  else msg("You feel weaker!!!",CLR_LIGHTRED);
  }

/* ***** The party, and the player's summoned snakes; the old log shouted HIT !!! for each blow ***** */

void log_ally_struck(NPC_REC* attacker,NPC_REC* member,Int32 damage) {
  if (improved()) msgf(CLR_LIGHTRED,"%s hits your %s for %d",the_name(attacker->name,LINE_START),member->name,damage);
  }

void log_ally_bolted(NPC_REC* attacker,NPC_REC* member,Int32 damage) {
  if (improved())
    msgf(CLR_LIGHTRED,"%s's bolt hits your %s for %d",the_name(attacker->name,LINE_START),member->name,damage);
  }

void log_ally_bolt_miss(NPC_REC* attacker,NPC_REC* member) {
  if (improved()) msgf(CLR_GRAY,"%s's bolt misses your %s",the_name(attacker->name,LINE_START),member->name);
    else prompt("Fizzle. . .");
  }

void log_ally_hit(NPC_REC* member,NPC_REC* target,Int32 damage) {
  if (improved()) msgf(CLR_LIGHTCYAN,"Your %s hits %s for %d",member->name,the_name(target->name,MID_LINE),damage);
  }

void log_ally_spell_hit(NPC_REC* member,NPC_REC* target,Int32 damage) {
  if (improved())
    msgf(CLR_LIGHTCYAN,"Your %s's spell hits %s for %d",member->name,the_name(target->name,MID_LINE),damage);
  }

void log_ally_spell_miss(NPC_REC* member,NPC_REC* target) {
  if (improved()) msgf(CLR_GRAY,"Your %s's spell misses %s",member->name,the_name(target->name,MID_LINE));
    else prompt("Fizzle. . .");
  }

void log_ally_asleep(NPC_REC* attacker,NPC_REC* member) {
  if (improved()) msgf(CLR_LIGHTBLUE,"%s puts your %s to sleep",the_name(attacker->name,LINE_START),member->name);
    else prompt("Party slept!!!");
  }

void log_ally_afraid(NPC_REC* attacker,NPC_REC* member) {
  if (improved()) msgf(CLR_LIGHTBLUE,"%s terrifies your %s",the_name(attacker->name,LINE_START),member->name);
    else prompt("Party scared!!!");
  }

void log_ally_fallen(NPC_REC* member) {
  if (improved()) msgf(CLR_LIGHTRED,"Your %s has fallen!",member->name);
  }

/* A party healer heals the player; the old log gave do_heal()'s line before its own */
void log_ally_heals_you(NPC_REC* healer,Int32 amount) {
  if (improved()) {
    msgf(CLR_LIGHTGREEN,"Your %s heals you for %d HP",healer->name,amount);
    return;
    }

  msgf(CLR_WHITE,"Healed %d",amount);
  msgf(CLR_LIGHTGREEN,"%s healed you for %d",healer->name,amount);
  }

void log_ally_heals(NPC_REC* healer,NPC_REC* member,Int32 amount) {
  if (improved()) msgf(CLR_LIGHTGREEN,"Your %s heals your %s for %d HP",healer->name,member->name,amount);
    else msgf(CLR_LIGHTGREEN,"%s healed %s for %d",healer->name,member->name,amount);
  }

void log_snake_bite(NPC_REC* target,Int32 damage) {
  if (improved()) msgf(CLR_LIGHTCYAN,"Your snake bites %s for %d",the_name(target->name,MID_LINE),damage);
  }

/* ***** Effects over time ***** */

void log_poison_damage(Int32 damage) {
  if (improved()) msgf(CLR_LIGHTPURPLE,"You take %d poison damage",damage);
    else msgf(CLR_LIGHTPURPLE,"%d poison damage",damage);
  }

/* Damage over time: burning, and wounds that keep hurting */
void log_wound_damage(Int32 damage) {
  if (improved()) msgf(CLR_LIGHTRED,"Your wounds hurt you for %d",damage);
    else msgf(CLR_LIGHTRED,"%d DOT damage",damage);
  }

void log_fear_over(void) {
  if (improved()) msg("You're no longer afraid",CLR_YELLOW);
    else msg("No longer afraid",CLR_YELLOW);
  }

void log_confusion_over(void) {
  if (improved()) msg("You're no longer confused",CLR_YELLOW);
    else msg("Your head clears",CLR_YELLOW);
  }

void log_protection_over(void) {
  if (improved()) msg("Your magical protection fades",CLR_GRAY);
    else msg("Protection fades",CLR_GRAY);
  }

/* The player becomes monsters[monster] */
void log_polymorph(Int32 monster) {
  if (improved()) msgf(CLR_WHITE,"You transform into %s",a_name(monsters[monster].name));
    else msgf(CLR_WHITE,"Became %s",monsters[monster].name);
  }

void log_polymorph_failed(void) {
  if (improved()) msg("Your transformation fails",CLR_GRAY);
    else msg("Fizzle...",CLR_WHITE);
  }

void log_polymorph_ending(Int32 turns) {
  if (improved()) msgf(CLR_WHITE,"Your %s form ends in %d turns",monsters[player.effect[PE_POLYMORPH]].name,turns);
    else msgf(CLR_WHITE,"Poly ends %d turns",turns);
  }

void log_polymorph_ended(void) {
  if (improved()) msg("You return to your normal form",CLR_WHITE);
    else msg("Polymorph ended",CLR_WHITE);
  }

/* ***** Healing, and items that hurt ***** */

/* amount is the healing given, healed what the player gained of it */
void log_heal(Int32 amount,Int32 healed) {
  if (improved()) msgf(CLR_LIGHTGREEN,"You heal %d HP (%d/%d)",healed,player.chp,get_mhp());
    else msgf(CLR_WHITE,"Healed %d",amount);
  }

void log_item_damage(Int32 damage) {
  if (improved()) msgf(CLR_LIGHTRED,"You take %d damage",damage);
    else msgf(CLR_LIGHTRED,"Took %d damage",damage);
  }

void log_item_poison(void) {
  if (improved()) msg("You're poisoned",CLR_LIGHTPURPLE);
    else msg("Poisoned!",CLR_LIGHTPURPLE);
  }

/* ***** Traps and hazards ***** */

void log_dart_trap(Int32 damage) {
  if (improved()) msgf(CLR_LIGHTCYAN,"A dart trap hits you for %d",damage);
    else msg("Hit by a dart!",CLR_LIGHTCYAN);
  }

void log_poison_dart(void) {
  if (improved()) msg("A poisoned dart hits you",CLR_LIGHTPURPLE);
    else msg("Poisoned dart!!!",CLR_LIGHTPURPLE);
  }

void log_fire_trap(void) {
  if (improved()) msg("A fire trap sets you ablaze",CLR_LIGHTCYAN);
    else msg("Burning!",CLR_LIGHTCYAN);
  }

void log_blind_trap(void) {
  if (improved()) msg("A flash of light blinds you",CLR_LIGHTCYAN);
    else msg("Bright flash of light!",CLR_LIGHTCYAN);
  }

/* After the fall, on the level below */
void log_pit_trap(void) {
  //if (improved()) msgf(CLR_LIGHTCYAN,"You fall through a pit to level %d",player.d_level);
  if (improved()) msg("You fall through a pit",CLR_LIGHTCYAN);
    else msg("Fell down a pit",CLR_LIGHTCYAN);
  }

void log_root_trap(void) {
  if (improved()) msg("A trap roots you in place",CLR_LIGHTCYAN);
    else prompt("Cannot move!!");
  }

void log_sleep_trap(void) {
  if (improved()) msg("A sleep trap puts you to sleep",CLR_LIGHTCYAN);
    else prompt("Slept!!");
  }

void log_force_field(Int32 loss) {
  if (improved()) msgf(CLR_LIGHTRED,"The force field shocks you for %d",loss);
    else msg("ZZZZAAAAPPPP!!!!!",CLR_LIGHTRED);
  }

void log_fire_tile(Int32 loss) {
  if (improved()) msgf(CLR_LIGHTRED,"The flames burn you for %d",loss);
    else msg("SIZZLE!!!!!",CLR_LIGHTRED);
  }

void log_water_tile(Int32 loss) {
  if (improved()) msgf(CLR_LIGHTRED,"You nearly drown and lose %d HP",loss);
    else msg("GLUB GLUB!!!",CLR_LIGHTRED);
  }

static Int32 is_player_trap(Int32 trap_type) {
  return (trap_type >= TRAP_SMALL_EXPLOSIVE && trap_type <= TRAP_CONFUSION);
  }

/* A creature, or nothing, sets off one of the player's traps */
void log_trap_sprung(NPC_REC* creature,Int32 trap_type) {
  char* kind;

  if (!improved() || !is_player_trap(trap_type)) return;

  kind = TRAP_KINDS[trap_type - TRAP_SMALL_EXPLOSIVE];
  if (creature != NULL) msgf(CLR_LIGHTRED,"%s sets off your %s trap",the_name(creature->name,LINE_START),kind);
    else msgf(CLR_LIGHTRED,"Your %s trap goes off",kind);
  }

/* The old log's line for an explosive trap, as its blast showed */
void log_explosion(void) {
  if (!improved()) msg("Explosive trap triggered",CLR_LIGHTRED);
  }

/* The player is caught in their own trap's blast; a fire trap's burns over time, with no damage at once */
void log_trap_blast(Int32 trap_type,Int32 damage) {
  Int32 trap;

  if (!is_player_trap(trap_type)) return;

  trap = trap_type - TRAP_SMALL_EXPLOSIVE;
  if (!improved()) msg(TRAP_SHOUTS[trap],CLR_LIGHTRED);
    else if (trap_type == TRAP_FIRE) msg("Your own fire trap sets you ablaze",CLR_LIGHTRED);
    else msgf(CLR_LIGHTRED,"Your own %s trap hits you for %d",TRAP_KINDS[trap],damage);
  }

/* ***** Levels, attributes and skills ***** */

/* Before the level is gained */
void log_level_up(void) {
  if (improved()) msgf(CLR_WHITE,"You've reached level %d!",player.level + 1);
    else msg("Gained a Level !!!",CLR_WHITE);
  }

void log_hp_gain(Int32 amount) {
  if (improved()) msgf(CLR_LIGHTGREEN,"Your max HP rises to %d (+%d)",get_mhp(),amount);
    else msgf(CLR_LIGHTGREEN,"Gained %d Hit Points",amount);
  }

void log_mana_gain(Int32 amount) {
  if (improved()) msgf(CLR_LIGHTGREEN,"Your max mana rises to %d (+%d)",get_mmana(),amount);
    else msgf(CLR_LIGHTGREEN,"Gained %d Mana Points",amount);
  }

void log_stat_gain(ATTRIBUTE stat) {
  if (improved()) msgf(CLR_YELLOW,"Your %s rises to %d",ATTRIBUTE_NAMES[stat],attribute_value(stat));
    else msgf(CLR_YELLOW,"Gained %s",ATTRIBUTE_NAMES[stat]);
  }

static void improved_skill_gain(Int32 skill) {
  msgf(CLR_YELLOW,"You've become better at %s! (%d)",SKILL_NAMES[skill],get_skill(skill));
  }

/* A skill rises through use */
void log_skill_gain(Int32 skill) {
  if (!is_skill(skill)) return;

  if (improved()) improved_skill_gain(skill);
    else msgf(CLR_YELLOW,"Gained %s",SKILL_NAMES[skill]);
  }

/* A skill rises with a level; the old log knew only the skills up to Combat, by their old names */
void log_level_skill(Int32 skill) {
  if (!is_skill(skill)) return;

  if (improved()) improved_skill_gain(skill);
    else if (skill <= SK_COMBAT) msgf(CLR_YELLOW,"Gained %s",OLD_SKILL_NAMES[skill]);
  }

/* An altar's boon, after it is given: amount is the hit points or mana gained, or the skill raised */
void log_altar_boon(ALTAR_BOON boon,Int32 amount) {
  if (!improved()) {
    prompt((boon >= ALTAR_STRENGTH && boon < ALTAR_BOONS) ? OLD_ALTAR_BOONS[boon] : "Fizzle...");
    return;
    }

  switch (boon) {
    case ALTAR_STRENGTH:     log_stat_gain(ATTR_STRENGTH); break;
    case ALTAR_DEXTERITY:    log_stat_gain(ATTR_DEXTERITY); break;
    case ALTAR_CONSTITUTION: log_stat_gain(ATTR_CONSTITUTION); break;
    case ALTAR_HIT_POINTS:   log_hp_gain(amount); break;
    case ALTAR_MANA:         log_mana_gain(amount); break;
    case ALTAR_SKILL:        improved_skill_gain(amount); break;
    case ALTAR_CHARISMA:     log_stat_gain(ATTR_CHARISMA); break;
    default:                 msg("Nothing happens",CLR_GRAY);
    }
  }

void log_gold_found(Int32 gold) {
  if (improved()) msgf(CLR_YELLOW,"You find %d gold pieces",gold);
    else msgf(CLR_YELLOW,"Found %d Gold Pieces",gold);
  }

/* ***** Knowing and summoning creatures ***** */

/* Know Enemy names the first creature in a direction, or finds none */
void log_know_enemy(NPC_REC* creature) {
  if (creature == NULL) {
    if (improved()) msg("You sense nothing there",CLR_GRAY);
      else prompt("Fizzle. . .");
    return;
    }

  if (improved()) msgf(CLR_LIGHTGREEN,"You sense %s",a_name(creature->name));
    else prompt(creature->name);
  }

/* A creature called by a summoning special; the other summonings were silent */
void log_called(NPC_REC* ally) {
  if (improved()) msgf(CLR_LIGHTCYAN,"You summon %s to your side",a_name(ally->name));
    else msgf(CLR_LIGHTGREEN,"Called a %s",ally->name);
  }

void log_summoned(NPC_REC* ally) {
  if (improved()) msgf(CLR_LIGHTCYAN,"You summon %s to your side",a_name(ally->name));
  }

void log_summon_failed(void) {
  if (improved()) msg("Your summoning fails",CLR_GRAY);
    else msg("Fizzle",CLR_LIGHTRED);
  }

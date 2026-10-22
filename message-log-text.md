# Message log text

Every line the game writes to the message log, the box under the stats in the top right of the play screen.

> **Since this list was made**, the Improved Combat Logs preference moved the combat and event lines into [combatlog.c](combatlog.c). That file keeps the original text below for when the preference is Off. The file and line links below point to where each line was written in 7.0, before the move.

## How text reaches the box

- **`msg(text, colour)`** ([msg.c](msg.c)) adds a line. [showmessages.c](showmessages.c) draws it at `screen_width-215`, from y 110. Lines are 21 characters wide, and longer text wraps at spaces.
- **`msg_named()`** ([msg.c](msg.c)) does the same, but draws an item name in its rarity colour.
- **`prompt(text)`** ([prompt.c](prompt.c)) is `msg(text, 10)`, so all of its text is light green. Its old drawing code below the `return` never runs.
- **`promptNS(text)`** and any negative colour rewrite the newest line instead of adding one. Typed input uses this.
- **`showCard(title, text)`** ([specials.c:1571](specials.c:1571)) writes the title in white, then the text in yellow.
- **`clear_prompt()`** ([clearprompt.c](clearprompt.c)) does nothing. Its body is commented out.

Colours are palette indices: 1 Blue, 4 Red, 7 Light grey, 9 Light blue, 10 Light green, 11 Light cyan, 12 Light red, 13 Light magenta, 14 Yellow, 15 White.

`N` stands for a number and `<elem>` for the element suffix: empty, ` fire `, ` cold `, ` electric `, ` arcane ` or ` psionic `. The suffix has a space on both sides, so a line reads `OUCH! for 7 fire `.

---

## 1. Combat

### The player attacks

| Text | Colour | When | Source |
|---|---|---|---|
| `Hit for N` | Light green | Melee, bow, thrown and missile-spell damage (all go through `aoe()`) | [aoe.c:85](aoe.c:85) |
| `Crit hit for N` | Light green | The same, on a critical hit | [aoe.c:84](aoe.c:84) |
| `Hit for N` / `Crit hit for N` | Light green | Touch spells | [touch.c:55](touch.c:55) |
| `HIT !!!` | Light green | A bow or thrown hit, before its `Hit for N` line | [fire.c:65](fire.c:65) |
| `Miss . . .` | Light blue | Melee miss | [meleehit.c:83](meleehit.c:83) |
| `Missed. . .` | Light green | Bow, thrown or wand miss | [fire.c:89](fire.c:89), [firewand.c:80](firewand.c:80) |
| `HIT for N` | Red | Wand hit | [firewand.c:61](firewand.c:61) |
| `CRIT for N` | Red | Wand critical hit | [firewand.c:60](firewand.c:60) |
| `Paralyzed!` | Yellow | The Monk's paralysing blow | [meleehit.c:63](meleehit.c:63) |
| `Killed!` | White | The Monk's instant kill, a rare roll after a paralysing blow | [meleehit.c:67](meleehit.c:67) |
| `Fizzle. . .` | Light green | An area or touch spell misses a creature | [aoe.c:164](aoe.c:164), [touch.c:87](touch.c:87) |
| `Hand is empty` | Yellow | Throwing from an empty hand | [throw.c:50](throw.c:50) |

### Kills and experience

There is no "you killed X" line. A kill shows only as the experience it gives.

| Text | Colour | When | Source |
|---|---|---|---|
| `Exp=N` | White | Any kill (`dead_monster()` → `gain_exp()`) | [gainexp.c:30](gainexp.c:30) |
| `Exp=N` | Light green | A spell kill when the Spell Kills change is off | [spellkill.c:18](spellkill.c:18) |
| `<member> leveled` | White | A party member levels up from shared experience | [gainexp.c:44](gainexp.c:44) |

### The player's spells and items on creatures

| Text | Colour | When | Source |
|---|---|---|---|
| `Healed !!!` | Light green | Healing effect on a creature | [aoe.c:60](aoe.c:60), [thrownpotion.c:21](thrownpotion.c:21) |
| `Slept !!!` | Light green | Sleep effect | [aoe.c:98](aoe.c:98), [thrownpotion.c:29](thrownpotion.c:29) |
| `Scared !!!` | Light green | Fear effect | [aoe.c:106](aoe.c:106), [thrownpotion.c:34](thrownpotion.c:34) |
| `Blinded !!!` | Light green | Blind effect | [aoe.c:114](aoe.c:114), [thrownpotion.c:39](thrownpotion.c:39), [blind.c:47](blind.c:47) |
| `Confused !!!` | Light green | Confusion effect | [aoe.c:122](aoe.c:122), [thrownpotion.c:44](thrownpotion.c:44) |
| `Poisoned !!!` | Light green | Poison effect | [aoe.c:161](aoe.c:161), [thrownpotion.c:67](thrownpotion.c:67) |
| `SLEPT !!!` | Light green | Sleep spell | [dosleep.c:29](dosleep.c:29) |
| `SCARED!!!` | Light green | Fear spell | [fear.c:34](fear.c:34) |
| `Fizzle. . . ` (trailing space) | Light green | Blind, sleep or fear spell fails | [blind.c:64](blind.c:64), [dosleep.c:42](dosleep.c:42), [fear.c:44](fear.c:44) |
| `HIT !!!` | Light green | Area, super, drain and reflected-missile spells (no damage number) | [area9.c:32](area9.c:32), [area25.c:32](area25.c:32), [area49.c:49](area49.c:49), [super.c:33](super.c:33), [drain.c:30](drain.c:30), [r_missile.c:58](r_missile.c:58) |
| `Fizzle. . .` | Light green | Those spells miss | [area9.c:85](area9.c:85), [area25.c:85](area25.c:85), [area49.c:96](area49.c:96), [super.c:75](super.c:75), [drain.c:75](drain.c:75), [r_missile.c:99](r_missile.c:99) |
| `HIT !!!` | Light green | A snake summoned by the player bites | [movesnake.c:43](movesnake.c:43), [arenamovesnake.c:74](arenamovesnake.c:74) |
| `Charmed` | Light green | Charm succeeds | [charmmonster.c:33](charmmonster.c:33) |
| `Fizzle` | Light green | Charm fails | [charmmonster.c:26](charmmonster.c:26), [charmmonster.c:30](charmmonster.c:30), [charmmonster.c:38](charmmonster.c:38) |
| `Pacified` | White | One line for each creature pacified | [specials.c:1147](specials.c:1147) |
| `Called a <monster>` | Light green | Summon | [specials.c:334](specials.c:334) |
| `Fizzle` | Light red | Summon finds no room | [specials.c:338](specials.c:338) |
| `<monster name>` | Light green | Special 23 names the first creature in a direction | [specials.c:276](specials.c:276) |
| `Fizzle. . .` | Light green | Special 23 or 30 finds nothing | [specials.c:282](specials.c:282), [specials.c:367](specials.c:367) |
| `Mob no longer scared` | Light grey | A creature's fear wears off (towns and dungeons) | [cave.c:1270](cave.c:1270) |

### Creatures attack the player

| Text | Colour | When | Source |
|---|---|---|---|
| `OUCH! for N<elem>` | Light red | Melee hit | [enemyhit.c:177](enemyhit.c:177) |
| `DOT taken` | Light red | Melee hit from a damage-over-time creature | [enemyhit.c:174](enemyhit.c:174) |
| `Missed . . .` | Blue | Melee miss | [enemyhit.c:187](enemyhit.c:187) |
| `Dodged` | Light grey | The player dodges melee or a missile | [enemyhit.c:36](enemyhit.c:36), [enemymissile.c:68](enemymissile.c:68), [arena.c:321](arena.c:321) |
| `ZAP for N<elem>` | Light red | Ranged magic hit | [enemymissile.c:119](enemymissile.c:119), [arena.c:366](arena.c:366) |
| `Fizzle. . .` | Light green | Ranged magic misses | [enemymissile.c:147](enemymissile.c:147), [arena.c:395](arena.c:395) |
| `Ring Melted!!!` | Light green | A Reflection ring is destroyed (towns and dungeons) | [enemymissile.c:134](enemymissile.c:134), [enemymissile.c:139](enemymissile.c:139) |
| `Ring Melted!!` | Light green | The same, in the arena | [arena.c:382](arena.c:382), [arena.c:387](arena.c:387) |
| `Rooted!!!` | Light blue | A melee hit roots the player | [enemyhit.c:83](enemyhit.c:83) |
| `Slept!!!` | Light blue | A melee hit puts the player to sleep | [enemyhit.c:101](enemyhit.c:101) |
| `Poisoned!!!` | Light magenta | A melee hit poisons the player | [enemyhit.c:120](enemyhit.c:120) |
| `Scared!!!` | Light blue | A melee hit frightens the player | [enemyhit.c:139](enemyhit.c:139) |
| `Confused!!!` | Light blue | A melee hit confuses the player | [enemyhit.c:154](enemyhit.c:154) |
| `You feel weaker!!!` | Light red | Level drain | [enemyhit.c:163](enemyhit.c:163) |

### The arena and the party

| Text | Colour | When | Source |
|---|---|---|---|
| `HIT !!!` | Light green | A creature hits a party member | [arena.c:51](arena.c:51) |
| `HIT !!!` | Light green | A party member hits a creature | [arena.c:113](arena.c:113) |
| `HIT !!!` | Light green | A party member's missile hits | [arena.c:212](arena.c:212) |
| `HIT !!!` | Light green | A creature's missile hits a party member | [arena.c:404](arena.c:404) |
| `Fizzle. . .` | Light green | A party missile misses, or a creature's missile misses a party member | [arena.c:235](arena.c:235), [arena.c:423](arena.c:423) |
| `Party slept!!!` | Light green | A creature puts a party member to sleep | [arena.c:64](arena.c:64) |
| `Party scared!!!` | Light green | A creature frightens a party member | [arena.c:73](arena.c:73) |
| `<healer> healed you for N` | Light green | A party healer heals the player | [arenaheal.c:37](arenaheal.c:37) |
| `<healer> healed <member> for N` | Light green | A party healer heals a party member | [arenaheal.c:44](arenaheal.c:44) |
| `<item name>` | Yellow | The player steps onto an item | [arena.c:921](arena.c:921), [cave.c:1149](cave.c:1149) |
| `Overburdoned` (sic) | White | A step is refused because the load is too heavy | [arena.c:478](arena.c:478), [cave.c:279](cave.c:279) |

### Effects each turn

| Text | Colour | When | Source |
|---|---|---|---|
| `N poison damage` | Light magenta | Poison ticks on the player | [cave.c:1349](cave.c:1349), [arena.c:1068](arena.c:1068) |
| `N DOT damage` | Light red | Damage over time ticks on the player | [cave.c:1359](cave.c:1359), [arena.c:1078](arena.c:1078) |
| `No longer afraid` | Yellow | Fear ends | [cave.c:1370](cave.c:1370), [arena.c:1089](arena.c:1089) |
| `Your head clears` | Yellow | Confusion ends (towns and dungeons only; the arena counts it down silently) | [cave.c:1374](cave.c:1374) |
| `Protection fades` | Light grey | Protection ends | [cave.c:1296](cave.c:1296) |
| `Poly ends 10 turns`, then `Poly ends N turns` for 5 to 1 | White | Polymorph is running out | [tickpolymorph.c:20](tickpolymorph.c:20), [tickpolymorph.c:22](tickpolymorph.c:22) |
| `Polymorph ended` | White | Polymorph ends | [tickpolymorph.c:27](tickpolymorph.c:27) |
| `Became <monster>` | White | Polymorph starts | [specials.c:1127](specials.c:1127) |
| `Fizzle...` | White | Polymorph finds no monster | [specials.c:1133](specials.c:1133) |

### Healing

| Text | Colour | When | Source |
|---|---|---|---|
| `Healed N` | White | The player is healed (`do_heal()`) | [doheal.c:18](doheal.c:18) |
| `Healed N` | White | The Heal command (special 88) | [specials.c:1109](specials.c:1109) |
| `No healing item!!!` | White | Heal with nothing to heal with | [healing.c:76](healing.c:76) |
| `Took N damage` | Light red | Using an item that does damage | [specials.c:2095](specials.c:2095) |
| `Poisoned!` | Light magenta | Using a poisonous item | [specials.c:2123](specials.c:2123) |

---

## 2. Traps and hazards

### Traps the player steps on

| Text | Colour | Source |
|---|---|---|
| `Hit by a dart!` | Light cyan | [darttrap.c:20](darttrap.c:20) |
| `Poisoned dart!!!` | Light magenta | [poisontrap.c:27](poisontrap.c:27) |
| `Burning!` | Light cyan | [firetrap.c:19](firetrap.c:19) |
| `Bright flash of light!` | Light cyan | [blindtrap.c:19](blindtrap.c:19) |
| `Fell down a pit` | Light cyan | [pittrap.c:45](pittrap.c:45) |
| `Cannot move!!` | Light green | [roottrap.c:17](roottrap.c:17) |
| `Slept!!` | Light green | [sleeptrap.c:26](sleeptrap.c:26) |

### Tiles that halve HP

| Text | Colour | Tile | Source |
|---|---|---|---|
| `ZZZZAAAAPPPP!!!!!` | Light red | Force field (41, 145) | [cave.c:1191](cave.c:1191) |
| `SIZZLE!!!!!` | Light red | Fire (38) | [cave.c:1199](cave.c:1199) |
| `GLUB GLUB!!!` | Light red | Water (39) | [cave.c:1206](cave.c:1206) |

### Traps the player sets

A creature sets these off. The lines name what happens to the player when the blast reaches them. They are all light red, in [triggertrap.c](triggertrap.c).

| Text | Trap | Line |
|---|---|---|
| `Explosive trap triggered` | Any explosive trap | [23](triggertrap.c:23) |
| `OUCH!!!` | Large or huge explosive trap | [48](triggertrap.c:48), [72](triggertrap.c:72) |
| `BURNING!!!` | Fire trap | [97](triggertrap.c:97) |
| `ROOTED!!!` | Frost trap | [124](triggertrap.c:124) |
| `ZAP!!!` | Electric trap | [151](triggertrap.c:151) |
| `SLEPT!!!` | Sleep trap | [178](triggertrap.c:178) |
| `SCARED!!!` | Fear trap | [205](triggertrap.c:205) |
| `BLIND!!!` | Blind trap | [232](triggertrap.c:232) |
| `POISONED!!!` | Poison trap | [260](triggertrap.c:260) |
| `CONFUSED!!!` | Confusion trap | [288](triggertrap.c:288) |
| `Could not set` (light grey) | Setting a trap fails | [specials.c:2138](specials.c:2138) |

---

## 3. Other events

### Level-ups and skills

| Text | Colour | When | Source |
|---|---|---|---|
| `Gained a Level !!!` | White | Level-up | [advancement.c:250](advancement.c:250) |
| `Gained N Hit Points` | Light green | Level-up | [advancement.c:258](advancement.c:258) |
| `Gained N Mana Points` | Light green | Level-up | [advancement.c:280](advancement.c:280), [advancement.c:291](advancement.c:291) |
| `Gained Strength` … `Gained Charisma` | Yellow | A stat rises at level-up | [advancestat.c:20](advancestat.c:20)–25 |
| `Gained Missile`, `Edge`, `Pole`, `Blunt`, `Scouting`, `Language`, `Lore`, `Magic`, `Healing`, `Medicine`, `Riding`, `Boating`, `Oracle`, `Charm`, `Survival`, `Combat` | Yellow | A skill rises at level-up (16 old names) | [advancement.c:15](advancement.c:15)–30 |
| `Gained Bow`, `1H Sword`, `Spear`, `Mace`, `Scouting` … `Combat`, `Dagger`, `2H Sword`, `Axe`, `Flail`, `Crossbow`, `Trident`, `Poleaxe`, `Thrown` | Yellow | A skill rises through use (24 names) | [showgain.c:15](showgain.c:15)–38 |
| `STR increased`, `DEX increased`, `CON increased`, `HP increased`, `Mana increased`, `Skill increased`, `CHR increased` | Light green | Special 31, from an altar | [specials.c:378](specials.c:378)–404 |
| `Fizzle...` | Light green | Special 31 rolls nothing | [specials.c:408](specials.c:408) |

### Potions, scrolls and items

| Text | Colour | Source |
|---|---|---|
| `You feel enlightened` | White | [specials.c:707](specials.c:707) |
| `You feel more energized` | White | [specials.c:717](specials.c:717) |
| `You feel fearful` | White | [specials.c:777](specials.c:777) |
| `Hmmmm, who turned off the lights?` | Yellow | [specials.c:789](specials.c:789) |
| `A fog clouds your mind` | Yellow | [specials.c:800](specials.c:800), [specials.c:936](specials.c:936) |
| `You seem to think clearly again` | Yellow | [specials.c:805](specials.c:805) |
| `You seem much braver` | Yellow | [specials.c:810](specials.c:810) |
| `Your weapon glows briefly` / `Your armor glows briefly` | White | [specials.c:834](specials.c:834), [specials.c:842](specials.c:842) |
| `Your weapon turns black` / `Your armor turns black` | Red | [specials.c:854](specials.c:854), [specials.c:862](specials.c:862) |
| `Item glows briefly` | White | [specials.c:872](specials.c:872), [specials.c:886](specials.c:886) |
| `Fizzle` | Light blue | [specials.c:896](specials.c:896), [teleport.c:34](teleport.c:34) |
| `Entire inventory  glows briefly` (double space) | White | [specials.c:908](specials.c:908) |
| `10 Torches added to inventory` | White | [specials.c:913](specials.c:913) |
| `Your body feels different` | White | [specials.c:924](specials.c:924) |
| `<item name>` | Yellow | The identify scroll names the item ([specials.c:171](specials.c:171)) |
| `The Triad is Incomplete` | Light green | [specials.c:662](specials.c:662), [specials.c:1192](specials.c:1192), [specials.c:1203](specials.c:1203), [specials.c:1213](specials.c:1213) |
| `Fizzle. . .` | Red | An enchantment won't take ([specials.c:2031](specials.c:2031)) |
| `Too full to eat any more` | Light green | [cave.c:179](cave.c:179), [eat.c:20](eat.c:20), [upland.c:305](upland.c:305), [useitem.c:38](useitem.c:38), [upland.c:341](upland.c:341) |
| `Nothing to Eat!!!` | Light green | [eat.c:40](eat.c:40) |

### Cards (`showCard()`, title in white, text in yellow)

All are in [specials.c:1608](specials.c:1608)–1821. Good cards: Life / Double your life, Rich / Gain 1,000,000 gold, Offense / Weapon gains +1, Defense / Armor gains +1, Experience / Gain 1 level, Skilled / Gain +5 in all skills, Buffed / Gain +5 to all stats, Enhanced / Gain 1 socket on equipment, Elevated / Instant level 35 (or Gain 5 levels), Spellcraft / Learn all class spells, Enlightened / Learn all message/lore, Survival / Gain +1 on all equipment, Craftsman / Learn all recipes.

Bad cards: Death / Death!!!, Poverty / Lose all wordly possessions (sic), Dullness / Weapon loses -1, Rusty / Armor loses -1, Tripped / Lose 1 level, Unskilled / Lose -5 in all skills, Unbuffed / Lose -5 to all stats, Disenhanted (sic) / Lose 1 socket on equipment, Knockback / Reduced to level 1, Forgetfull (sic) / Forget all known spells, Senility / Forget all message/lore, Archaic / Lose -1 on all equipment, Babbling / Forget all recipes, Blank / Blank card.

### Altars, gold, pack and people

| Text | Colour | Source |
|---|---|---|
| `Activating Altar`, `Altar already used`, `Runestone already used`, `Nothing to activate` | Yellow | [activate.c](activate.c) |
| `You sense your surroundings`, `You feel more skillful`, `Everything glows briefly` | White | [activate.c](activate.c) |
| `Found N Gold Pieces` | Yellow | [doget.c:50](doget.c:50), [checkautos.c:29](checkautos.c:29) |
| `Picked up torch` | Light grey | [checkautos.c:22](checkautos.c:22) |
| `Packed <item>` | White, name in rarity colour (Improved Interface), or all in the item's colour | [packit.c:51](packit.c:51), [packit.c:53](packit.c:53) |
| `Pack contains N items` | White | [packit.c:61](packit.c:61) |
| `Pack full !!!` | Yellow | [packit.c:67](packit.c:67) |
| `Spell learned`, `Too advanced`, `Not enough mana`, `Failed`, `Invalid Item` | Light grey or light red | [transcribe.c](transcribe.c) |
| `Not enough Mana!!` | Yellow | [cast_spell.c:75](cast_spell.c:75) |
| `Cast cancelled` | Light grey | [cast_spell.c:29](cast_spell.c:29) |
| `Words do Not work Here!!` | Light red | [cave.c:1078](cave.c:1078) |
| `House repossessed!!!` | Light red | [newmonth.c:34](newmonth.c:34) |
| `Not enough gold` | Yellow | [cave.c:619](cave.c:619) |
| `Nothing found` | Yellow | [upland.c:395](upland.c:395) |
| `Tardis Programmed` | Light green | [upland.c:455](upland.c:455) |
| `Saved as <name>` | White | [gamemenu.c:1055](gamemenu.c:1055) |
| `Equipped:` then each item's name, `Saved`, `Cancelled` | White / light grey | [loadgear.c](loadgear.c), [savegear.c](savegear.c) |
| `No one there ! ! !` | Yellow | Talk or join with nobody there ([cave.c:406](cave.c:406), [cave.c:523](cave.c:523)) |
| `Die Scum!!!` | Light green | Talk to a hostile creature ([cave.c:411](cave.c:411)) |
| `Hello!!` | Light blue | Talk to a friendly creature again ([cave.c:440](cave.c:440)) |
| `Trick or Treat` | White | [cave.c:427](cave.c:427) |
| `I would be Honored!!!` | White | A creature joins ([cave.c:467](cave.c:467)) |
| `Your party seems full as it is!!` | Yellow | [cave.c:488](cave.c:488) |
| `No Thanks.` / `No Way Scum!!!` | Yellow | A creature refuses to join ([cave.c:532](cave.c:532), [cave.c:537](cave.c:537)) |

### Input prompts (not events)

`Attack who ?`, `Talk to who?`, `Ask who to join?`, `Dir ?`, `Run dir?`, `Select target`, `<L>eft, or <R>ight hand ?`, `Spell ? _` (and `Spell ? <name>_`), `Say ? _`, `Function ? _`, `Desc ? _`, `How long ? _`, `Equip set (0-9)?`, `Save set (0-9)?`, `Press <ESC> to end view mode`, `Stocking Treasure...`, the `~` location readout (`Region    : N`, `Region X,Y: x,y`, `X,Y       : x,y`), the wizard password and `key=N` (ShowKeys).

---

## 4. Problems found

1. **Debug lines left in.** Every turn in a town or dungeon, each poisoned creature writes `here`, `poison=N` and `done` ([cave.c:1275](cave.c:1275)–1281). The Cube of Force writes `dir=x,y`, then `Cannot move` or `Move base`, for every square around the player ([specials.c:1225](specials.c:1225)–1230).
2. **Dodging writes two lines.** A dodge sets the hit chance to 0 but still reaches the miss branch. Melee then reads `Dodged` followed by `Missed . . .`, and a missile reads `Dodged` followed by `Fizzle. . .`.
3. **Kills have no line of their own.** A kill shows only as `Exp=N`.
4. **The same event is worded in several ways:**
   - The player's hits: `Hit for N` (light green), the wand's `HIT for N` (red), and `HIT !!!` with no number from area spells, the party and snakes.
   - Misses: `Miss . . .`, `Missed . . .` and `Missed. . .`.
   - Fizzles: `Fizzle`, `Fizzle...`, `Fizzle. . .` and `Fizzle. . . `.
   - `Ring Melted!!!` in towns and dungeons, but `Ring Melted!!` in the arena.
   - Status lines: `Slept!!!`, `Slept!!`, `Slept !!!`, `SLEPT !!!` and `SLEPT!!!`.
5. **Skill names differ.** A skill gained at level-up uses the 16 old names (`Gained Missile`, `Edge`, `Pole`, `Blunt`). The same skill gained through use reads `Gained Bow`, `1H Sword`, `Spear`, `Mace`. Level-up prints nothing for skills 17 to 24.
6. **Typos:** `Overburdoned`, `Forgetfull`, `Disenhanted`, `wordly`, and the double space in `Entire inventory  glows briefly`.
7. **Long lines wrap.** Lines hold 21 characters. `Hmmmm, who turned off the lights?`, `Your party seems full as it is!!`, `<healer> healed <member> for N` and `Entire inventory  glows briefly` each take two or three lines.

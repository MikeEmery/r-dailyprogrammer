#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <utility>
#include <algorithm>
#include <cassert>

using namespace std;

enum DamageType {
  NORMAL,
  FIRE,
  WATER,
  ELECTRIC,
  GRASS,
  ICE,
  FIGHTING,
  POISON,
  GROUND,
  FLYING,
  PSYCHIC,
  BUG,
  ROCK,
  GHOST,
  DRAGON,
  DARK,
  STEEL,
  FAIRY,
  UNKNOWN
};

typedef pair<DamageType, DamageType> DamagePair;

const map<string, DamageType> NAME_DMG_MAP = {
  {"normal", DamageType::NORMAL},
  {"fire", DamageType::FIRE},
  {"water", DamageType::WATER},
  {"electric", DamageType::ELECTRIC},
  {"grass", DamageType::GRASS},
  {"ice", DamageType::ICE},
  {"fighting", DamageType::FIGHTING},
  {"poison", DamageType::POISON},
  {"ground", DamageType::GROUND},
  {"flying", DamageType::FLYING},
  {"psychic", DamageType::PSYCHIC},
  {"bug", DamageType::BUG},
  {"rock", DamageType::ROCK},
  {"ghost", DamageType::GHOST},
  {"dragon", DamageType::DRAGON},
  {"dark", DamageType::DARK},
  {"steel", DamageType::STEEL},
  {"fairy", DamageType::FAIRY}
};

const map<DamagePair, float> DMG_PAIR_MAP = {
  {DamagePair(DamageType::NORMAL, DamageType::ROCK), 0.5},
  {DamagePair(DamageType::NORMAL, DamageType::GHOST), 0.0},
  {DamagePair(DamageType::NORMAL, DamageType::STEEL), 0.5},

  {DamagePair(DamageType::FIRE, DamageType::FIRE), 0.5},
  {DamagePair(DamageType::FIRE, DamageType::WATER), 0.5},
  {DamagePair(DamageType::FIRE, DamageType::GRASS), 2.0},
  {DamagePair(DamageType::FIRE, DamageType::ICE), 2.0},
  {DamagePair(DamageType::FIRE, DamageType::BUG), 2.0},
  {DamagePair(DamageType::FIRE, DamageType::ROCK), 0.5},
  {DamagePair(DamageType::FIRE, DamageType::DRAGON), 0.5},
  {DamagePair(DamageType::FIRE, DamageType::STEEL), 0.5},

  // too lazy to copy everything, complete list is here http://pokemondb.net/type
};

DamageType parseSource(const string& arg) {
  auto foundDamageType = DamageType::UNKNOWN;

  if (NAME_DMG_MAP.count(arg)) {
    foundDamageType = NAME_DMG_MAP.at(arg);
  }

  assert(foundDamageType != DamageType::UNKNOWN);

  return foundDamageType;
}

vector<DamageType> parseTarget(string& arg) {
  vector<DamageType> damage;

  return damage;
}

const char WHITESPACE = ' ';
const string ltrim(const string& str) {
  int startPos = 0;
  while(str[startPos] == WHITESPACE) {
    startPos++;
  }

  return str.substr(startPos, str.length()-startPos);
}

const string rtrim(const string& str) {
  int lastPos = str.length() - 1;
  while(str[lastPos] == WHITESPACE) {
    lastPos--;
  }

  return str.substr(0, lastPos+1);
}

const string trim(const string& str) {
  return ltrim(rtrim(str));
}

const auto SEPARATOR = string("->");
pair<string, string> separateSourceAndTarget(const string& str) {
  const auto pos = str.find(SEPARATOR);
  const string firstPart = str.substr(0, pos);
  const string secondPart = str.substr(pos+SEPARATOR.length());

  return pair<string, string>(trim(firstPart), trim(secondPart));
}

vector<string> splitTarget(const string& raw, vector<string>& acc, int start=0) {
  if (start >= raw.length()) {
    return acc;
  }

  while(raw[start] == WHITESPACE && start < raw.length()) {
    start++;
  }

  int end = start;
  while(raw[end] != WHITESPACE && end < raw.length()) {
    end++;
  }

  if (start == end) {
    return acc;
  }
  
  string token = raw.substr(start, end-start);
  acc.push_back(token);
  return splitTarget(raw, acc, end+1);
}

vector<pair<DamageType, vector<DamageType>>> parseInput(string fileName, DamageType& sourceDamage, vector<DamageType>& targetDamage) {
  ifstream inputFile(fileName);
  string line;

  auto result = vector<pair<DamageType, vector<DamageType>>>();

  while(getline(inputFile, line)) {
    pair<string, string> separated = separateSourceAndTarget(line);
    vector<string> targetDmgRaw;
    auto intermediary = pair<string, vector<string>>(separated.first, splitTarget(separated.second, targetDmgRaw));

    vector<DamageType> targetDmg;
    transform(targetDmgRaw.begin(), targetDmgRaw.end(), back_inserter(targetDmg), [](string damageName) {
      return parseSource(damageName);
    });

    auto parsed = pair<DamageType, vector<DamageType>>(parseSource(intermediary.first), targetDmg);
    result.push_back(parsed);
  }

  return result;
}

int main(const int argc, char **argv) {
  DamageType sourceDamage;
  vector<DamageType> targetDamage;

  vector<pair<DamageType, vector<DamageType>>> data = parseInput(argv[1], sourceDamage, targetDamage);
  for(auto item : data) {
    double multiplier = 1.0;
    for_each(item.second.begin(), item.second.end(), [&item, &multiplier](DamageType dmg) {
      double newMultiplier = 1.0;
      auto key = DamagePair(item.first, dmg);
      if (DMG_PAIR_MAP.count(key) > 0) {
        newMultiplier = DMG_PAIR_MAP.at(key);
      }
      multiplier *= newMultiplier;
    });
    cout << multiplier << "x" << endl;
  }
  return 0;
}

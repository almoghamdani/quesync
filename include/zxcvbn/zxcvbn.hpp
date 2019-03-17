#ifndef __ZXCVBN__ZXCVBN_HPP
#define __ZXCVBN__ZXCVBN_HPP

#include <zxcvbn/feedback.hpp>
#include <zxcvbn/scoring.hpp>
#include <zxcvbn/time_estimates.hpp>
#include <zxcvbn/util.hpp>
#include <zxcvbn/matching.hpp>

#include <string>
#include <vector>

namespace zxcvbn {

struct ZxcvbnResult {
  ScoringResult scoring;
  AttackTimes attack_times;
  Feedback feedback;
};

ZxcvbnResult zxcvbn(const std::string & password, const std::vector<std::string> & user_inputs)
{
  ZxcvbnResult res;
  std::vector<std::string> sanitized_inputs;
  for (auto user_input = user_inputs.begin(); user_input != user_inputs.end(); user_input++) {
    sanitized_inputs.push_back(zxcvbn::util::ascii_lower(*user_input));
  }

  auto matches = zxcvbn::omnimatch(password, sanitized_inputs);
  res.scoring = zxcvbn::most_guessable_match_sequence(password, matches, false);

  std::vector<zxcvbn::Match> sequence;
  std::move(res.scoring.sequence.begin(), res.scoring.sequence.end(),
            std::back_inserter(sequence));

  res.attack_times = estimate_attack_times(res.scoring.guesses);
  res.feedback = get_feedback(res.attack_times.score, sequence);

  return res;
}

}

#endif

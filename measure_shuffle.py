import subprocess
import re

from itertools import permutations

def permute_all(nums):
    nums = list(permutations(nums, len(nums)))
    return nums

def chiSquared (obs, exps):
    """
    Chi-squared Test Statistic X^2
    X^2 = \sum_i^n (obs_i - exps_i)^2 / exps_i
    - obs_i: the number of observations of type i
    - exps_i: the expected count of type i
    ---
    input:
    - obs: list of observations
    - exps: list of expectations

    output:
    chi-sum squared sum
    """
    chiSum = 0.0
    for ob, exp in zip(obs, exps):
        chiSum += ((ob - exp) ** 2) / exp

    return chiSum

if __name__ == '__main__':
    # define inputs
    test_count = int(1e6)
    inputs = "new\nit 1\nit 2\nit 3\nit 4\n"
    for _ in range(test_count):
        inputs += "shuffle\n"
    inputs += "free\nquit\n"

    # define process
    cmd = "./qtest -v 3"
    clist = cmd.split()

    # https://docs.python.org/3/library/subprocess.html#subprocess.run
    completedProcess = subprocess.run(clist, capture_output=True, text=True, input=inputs)

    # process output
    out = completedProcess.stdout
    startIdx = out.find("l = [1 2 3 4]")
    endIdx = out.find("l = NULL")
    seqs = out[startIdx + 14: endIdx]
    
    # use RE to fast match all the seqs (any four digits with space separated)
    regex = re.compile(r'\d \d \d \d')
    result = regex.findall(seqs) # --> ['2 3 1 4', ...]

    # find all shuffle possibility
    shuffle_arr = [str(i) for i in range(1, 5)]
    shuffle_perm = permute_all(shuffle_arr)
    shuffle_dict = {}

    for s in shuffle_perm:
        s_str = ' '.join(s)
        shuffle_dict[s_str] = 0

    # count each outcome
    for r in result:
        shuffle_dict[r] += 1
    
    # statistics
    exps = [ max(test_count // len(shuffle_dict), 1) ] * len(shuffle_dict)
    chiSquaredSum = chiSquared(shuffle_dict.values(), exps)

    # significance level (alpha): probability of alternative hypothesis (H1) -- shuffle is not uniform distribution
    # usually alpha = 0.05, chi-Squared sum, should be less than P_Table(df, alpha)
    # for degrees of freedom (df) = 23
    P_uniform = 35.172

    print(f"Test time: {test_count}")
    print(f"Expectation: {exps[0]}")
    if (test_count <= 100):
        print(f"Observation: {shuffle_dict.values()}")
    
    print(f"Chi-Squared Sum: {chiSquaredSum}")
    if (chiSquaredSum < P_uniform):
        print("Probabily uniform")
    else:
        print("Not Uniform")

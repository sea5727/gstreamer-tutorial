

18.191.223.12

simple-example 0 $(host -4 -t A stun.stunprotocol.org | awk '{ print $4 }') 3478


## nice_agent_set_remote_credentials
remote로부터 받은 ufrag와 pwd 를 셋 한다

## nice_agent_set_remote_candidates
remote 로부터 받은 candidate들을 등록한다.
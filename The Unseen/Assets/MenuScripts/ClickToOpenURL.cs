using System.Collections;
using System.Collections.Generic;
using UnityEngine;


public class ClickToOpenURL : MonoBehaviour {

    public void OpenCreditWebsite()
    {
        Application.OpenURL("https://docs.google.com/document/d/1lk7VkCOXexrQFOWy2o1sRwc58Vbzq-52rKm_Q1EUZfY/edit?usp=sharing");
    }

    public void OpenControlWebsite()
    {
        Application.OpenURL("https://docs.google.com/document/d/1M17aDkZhCjX4QmSkjVNKMVdUpeyBnwrJU_INt8VZtmE/edit?usp=sharing");
    }
}

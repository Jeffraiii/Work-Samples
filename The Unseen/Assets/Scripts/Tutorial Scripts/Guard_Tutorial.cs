using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class Guard_Tutorial : MonoBehaviour {

    public GameObject player;
    public GameObject PopupText;
    public Text theText;

    void Start () {
        theText.text = "Now the guards will be patrolling";
	}

	void Update () {
        if (player.transform.position.z >= -10f)
        {
            PopupText.SetActive(false);
        }

        if (player.transform.position.z >= -9.2f && player.transform.position.z <= -7.8f)
        {
            theText.text = "Remain invisible and stealthy. They won't see you unless you make mistakes!";
        }

        if (player.transform.position.z >= 0f)
        {
            theText.text = "Toggle between views to gather information and find a way out like before" + "\r\n" +"Good Luck to you, Subject 90007.";
        }

	}
}

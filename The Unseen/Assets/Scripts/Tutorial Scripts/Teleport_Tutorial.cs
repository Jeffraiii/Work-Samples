using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class Teleport_Tutorial : MonoBehaviour {

    public GameObject player;
    public GameObject PopupText;
    public Text theText;
    private int playerChargeNum;
    void Start () {
        playerChargeNum = player.GetComponent<AnimationMove>().mCharges;
        theText.text = "Teleport is the only way for you to swtich between Dark/Light";
    }
	
	void Update () {
        playerChargeNum = player.GetComponent<AnimationMove>().mCharges;

        if (playerChargeNum == 0)
        {
            theText.text = "No charges, No teleport ...";
        }

        else if (playerChargeNum >= 0 && player.transform.position.z >= -12f && player.transform.position.z <= -11.5f)
        {
            PopupText.SetActive(false);
            theText.text = "These red boxes are teleport chargeups that allow you to perform teleport. Pick them up!";
        }
        else if (playerChargeNum > 0 && player.transform.position.z >= -11.5f && player.transform.position.z <= -7.25f)
        {
            theText.text = "Press SPACE/TRIANGLE to perform teleport and switch your states. "
                            + "Hold the buttons to charge the teleport and teleport further. ";
        }
        else if (player.transform.position.z >= -7.20f && player.transform.position.z <= 100f)
        {
            theText.text = "Charges you have are displayed on the top right corner of the screen"
                            + " Also, press O /◻︎◻︎ SQUARE◻︎ to toggle between your views";
        }
    }
}

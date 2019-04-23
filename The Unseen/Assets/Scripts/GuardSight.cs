using System.Collections;

using System.Collections.Generic;

using UnityEngine;

using UnityEngine.UI;



public class GuardSight : MonoBehaviour {

    public GameObject gameOverTextPanel;
    public Text gameOverText;
    private GameObject player;

    public float fieldOfView;
    public 

	// Use this for initialization

	void Start () {
        gameOverTextPanel.SetActive(false);
        player = GameObject.FindGameObjectWithTag("Player");

	}

	

	// Update is called once per frame

	void Update () {

        AnimationMove pm = player.GetComponent<AnimationMove>();

        Vector3 diff = player.transform.position - transform.position;

        if (Vector3.Angle(transform.forward, diff) < fieldOfView * 0.5f){

            Ray r = new Ray(transform.position, diff.normalized);

            RaycastHit hitArray;

            if (Physics.Raycast(r, out hitArray, 3.25f))

            {

                if (hitArray.collider == player.GetComponent<Collider>())

                {

                    if (pm.isVisible)

                    {
                        gameOverTextPanel.SetActive(true);
                        gameOverText.text = "YOU DIED!" + "\r\n" + "The Unseen Was Seen";
                        AkSoundEngine.PostEvent("Set_State_Failure", gameObject);

                    }

                }

            }

        }

    }

    private void OnCollisionEnter(Collision collision)
    {
        if(collision.gameObject.tag == "Player")
        {
            gameOverTextPanel.SetActive(true);
            gameOverText.text = "YOU DIED!" + "\r\n" + "The Unseen Was Touched";
        }
    }
}


using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class EndLevel : MonoBehaviour
{
    public GameObject player;
    public Text finalText;
    public Text preText;
    public Text preText1;
    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        
    }
    private void OnTriggerEnter(Collider other)
    {
        if (other.CompareTag("Player"))
        {
            DisplayScore();
        }
    }
    
    public void DisplayScore()
    {
        preText.enabled = false;
        preText1.enabled = false;
        finalText.enabled = true;
        finalText.text = "Your Score: " + player.GetComponent<PlayerMove>().score;
        player.GetComponent<Rigidbody>().constraints = RigidbodyConstraints.FreezeAll;
    }
}

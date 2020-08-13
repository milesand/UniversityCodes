using UnityEngine;

public class FighterController : MonoBehaviour {

    public float speed;
    public float bound;
    public float fireRate;
    public GameObject shot;
    public Transform shotLocation;
    public AudioClip shotSound;
    public AudioClip deathSound;

    private float fireCooldown = 0.0f;
    private Animator anim;
    private int dieTriggerHash = Animator.StringToHash("Die");
    private PolygonCollider2D pc;
    private AudioSource sfx;
    private Transform shots;
    private GameController gameController;

    public bool alive;

    void Start() {
        this.alive = true;
        this.anim = this.GetComponent<Animator>();
        this.pc = this.GetComponent<PolygonCollider2D>();
        this.pc.enabled = true;
        this.sfx = GameObject.FindWithTag("Audio_SFX").GetComponent<AudioSource>();
        this.shots = GameObject.FindWithTag("FighterShots").GetComponent<Transform>();
        this.gameController = GameObject.FindWithTag("GameController").GetComponent<GameController>();
	}
	
	void Update() {
        if (this.alive) {
            float movement = Input.GetAxisRaw("Horizontal");
            if (movement != 0.0f) {
                gameObject.transform.Translate(speed * movement * Time.deltaTime, 0.0f, 0.0f);
                if (gameObject.transform.position.x < -bound) {
                    gameObject.transform.position = new Vector3(-bound, -3.5f, 0.0f);
                }
                if (gameObject.transform.position.x > bound) {
                    gameObject.transform.position = new Vector3(bound, -3.5f, 0.0f);
                }
            }
            if (fireCooldown > 0.0f) {
                fireCooldown -= Time.deltaTime;
            }
            if (fireCooldown <= 0.0f && Input.GetButton("Fire1") && shots.childCount < 2) {
                this.fireCooldown = this.fireRate;
                Instantiate(shot, shotLocation.position, shotLocation.rotation, shots);
                this.sfx.PlayOneShot(this.shotSound);
            }
        }
	}

    void OnTriggerEnter2D(Collider2D other) {
        if (other.gameObject.tag == "Enemy") {
            this.Die();
        } else if (other.gameObject.tag == "EnemyShot") {
            Destroy(other.gameObject);
            this.Die();
        }
    }

    void Die() {
        this.pc.enabled = false;
        this.alive = false;
        this.sfx.PlayOneShot(this.deathSound);
        this.anim.SetTrigger(dieTriggerHash);
        this.gameController.FighterDied();
    }

    public void Disable() {
        this.GetComponent<SpriteRenderer>().enabled = false;
    }

    public void Activate() {
        this.GetComponent<SpriteRenderer>().enabled = true;
        this.alive = true;
        this.pc.enabled = true;
    }
}
